/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (C) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

// TODO: 
// - We should handle incoming terminates after we have terminated, so the other
//   side can warn that he didn't receive all bytes correctly.
// - Should the proby stuff also wait for candidate used acknowledgement?

#include <Swiften/FileTransfer/OutgoingJingleFileTransfer.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/typeof/typeof.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/JingleTransportPayload.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>
#include <Swiften/FileTransfer/FileTransferTransporter.h>
#include <Swiften/FileTransfer/FileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/ReadBytestream.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Network/TimerFactory.h>

#include <Swiften/Base/Log.h>

using namespace Swift;

static const int DEFAULT_BLOCK_SIZE = 4096;

OutgoingJingleFileTransfer::OutgoingJingleFileTransfer(
		const JID& toJID,
		JingleSession::ref session,
		boost::shared_ptr<ReadBytestream> stream,
		FileTransferTransporterFactory* transporterFactory,
		TimerFactory* timerFactory,
		IDGenerator* idGenerator,
		const JingleFileTransferFileInfo& fileInfo,
		const FileTransferOptions& options,
		CryptoProvider* crypto) :
			JingleFileTransfer(session, toJID, transporterFactory),
			idGenerator(idGenerator),
			stream(stream),
			fileInfo(fileInfo),
			options(options),
			contentID(idGenerator->generateID(), JingleContentPayload::InitiatorCreator),
			state(Initial),
			candidateAcknowledged(false) {

	setFileInfo(fileInfo.getName(), fileInfo.getSize());

	// calculate both, MD5 and SHA-1 since we don't know which one the other side supports
	hashCalculator = new IncrementalBytestreamHashCalculator(true, true, crypto);
	stream->onRead.connect(
			boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));

	waitForRemoteTermination = timerFactory->createTimer(5000);
	waitForRemoteTermination->onTick.connect(boost::bind(&OutgoingJingleFileTransfer::handleWaitForRemoteTerminationTimeout, this));
}

OutgoingJingleFileTransfer::~OutgoingJingleFileTransfer() {
	stream->onRead.disconnect(
			boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));
	delete hashCalculator;
}
	
void OutgoingJingleFileTransfer::start() {
	SWIFT_LOG(debug) << std::endl;
	if (state != Initial) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	setTransporter(transporterFactory->createInitiatorTransporter(getInitiator(), getResponder(), options));
	setState(GeneratingInitialLocalCandidates);
	transporter->startGeneratingLocalCandidates();
}

void OutgoingJingleFileTransfer::cancel() {
	terminate(JinglePayload::Reason::Cancel);
}

void OutgoingJingleFileTransfer::terminate(JinglePayload::Reason::Type reason) {
	SWIFT_LOG(debug) << reason << std::endl;

	if (state != Initial && state != GeneratingInitialLocalCandidates && state != Finished) {
		session->sendTerminate(reason);
	}
	stopAll();
	setFinishedState(getExternalFinishedState(reason), getFileTransferError(reason));
}

void OutgoingJingleFileTransfer::handleSessionAcceptReceived(
		const JingleContentID&, 
		JingleDescription::ref, 
		JingleTransportPayload::ref transportPayload) {
	SWIFT_LOG(debug) << std::endl;
	if (state != WaitingForAccept) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	if (JingleS5BTransportPayload::ref s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transportPayload)) {
		transporter->addRemoteCandidates(s5bPayload->getCandidates(), s5bPayload->getDstAddr());
		setState(TryingCandidates);
		transporter->startTryingRemoteCandidates();
	}
	else {
		SWIFT_LOG(debug) << "Unknown transport payload. Falling back." << std::endl;
		fallback();
	}
}

void OutgoingJingleFileTransfer::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) {
	SWIFT_LOG(debug) << std::endl;
	if (state == Finished) { SWIFT_LOG(warning) << "Incorrect state: " << state << std::endl; return; }

	stopAll();
	if (state == WaitForTermination) {
		waitForRemoteTermination->stop();
	}
	if (reason && reason->type == JinglePayload::Reason::Cancel) {
		setFinishedState(FileTransfer::State::Canceled, FileTransferError(FileTransferError::PeerError));
	}
	else if (reason && reason->type == JinglePayload::Reason::Success) {
		setFinishedState(FileTransfer::State::Finished, boost::optional<FileTransferError>());
	} 
	else {
		setFinishedState(FileTransfer::State::Failed, FileTransferError(FileTransferError::PeerError));
	}
}

void OutgoingJingleFileTransfer::handleTransportAcceptReceived(const JingleContentID&, JingleTransportPayload::ref transport) {
	SWIFT_LOG(debug) << std::endl;
	if (state != FallbackRequested) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	if (JingleIBBTransportPayload::ref ibbPayload = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport)) {
		startTransferring(transporter->createIBBSendSession(ibbPayload->getSessionID(), ibbPayload->getBlockSize().get_value_or(DEFAULT_BLOCK_SIZE), stream));
	} 
	else {
		SWIFT_LOG(debug) << "Unknown transport replacement" << std::endl;
		terminate(JinglePayload::Reason::FailedTransport);
	}
}

void OutgoingJingleFileTransfer::handleTransportRejectReceived(const JingleContentID &, boost::shared_ptr<JingleTransportPayload>) {
	SWIFT_LOG(debug) << std::endl;

	terminate(JinglePayload::Reason::UnsupportedTransports);
}

void OutgoingJingleFileTransfer::sendSessionInfoHash() {
	SWIFT_LOG(debug) << std::endl;

	JingleFileTransferHash::ref hashElement = boost::make_shared<JingleFileTransferHash>();
	hashElement->getFileInfo().addHash(HashElement("sha-1", hashCalculator->getSHA1Hash()));
	hashElement->getFileInfo().addHash(HashElement("md5", hashCalculator->getMD5Hash()));
	session->sendInfo(hashElement);
}

void OutgoingJingleFileTransfer::handleLocalTransportCandidatesGenerated(
		const std::string& s5bSessionID, const std::vector<JingleS5BTransportPayload::Candidate>& candidates, const std::string& dstAddr) {
	SWIFT_LOG(debug) << std::endl;
	if (state != GeneratingInitialLocalCandidates) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	fillCandidateMap(localCandidates, candidates);

	JingleFileTransferDescription::ref description = boost::make_shared<JingleFileTransferDescription>();
	fileInfo.addHash(HashElement("sha-1", ByteArray()));
	fileInfo.addHash(HashElement("md5", ByteArray()));
	description->setFileInfo(fileInfo);

	JingleS5BTransportPayload::ref transport = boost::make_shared<JingleS5BTransportPayload>();
	transport->setSessionID(s5bSessionID);
	transport->setMode(JingleS5BTransportPayload::TCPMode);
	transport->setDstAddr(dstAddr);
	foreach(JingleS5BTransportPayload::Candidate candidate, candidates) {
		transport->addCandidate(candidate);	
	}
	setState(WaitingForAccept);
	session->sendInitiate(contentID, description, transport);
}

void OutgoingJingleFileTransfer::fallback() {
	if (options.isInBandAllowed()) {
		SWIFT_LOG(debug) << "Trying to fallback to IBB transport." << std::endl;
		JingleIBBTransportPayload::ref ibbTransport = boost::make_shared<JingleIBBTransportPayload>();
		ibbTransport->setBlockSize(DEFAULT_BLOCK_SIZE);
		ibbTransport->setSessionID(idGenerator->generateID());
		setState(FallbackRequested);
		session->sendTransportReplace(contentID, ibbTransport);
	}
	else {
		SWIFT_LOG(debug) << "Fallback to IBB transport not allowed." << std::endl;
		terminate(JinglePayload::Reason::ConnectivityError);
	}
}

void OutgoingJingleFileTransfer::handleTransferFinished(boost::optional<FileTransferError> error) {
	SWIFT_LOG(debug) << std::endl;
	if (state != Transferring) { SWIFT_LOG(warning) << "Incorrect state: " << state << std::endl; return; }

	if (error) {
		terminate(JinglePayload::Reason::ConnectivityError);
	} 
	else {
		sendSessionInfoHash();

		// wait for other party to terminate session after they have verified the hash
		setState(WaitForTermination);
		waitForRemoteTermination->start();
	}
}

void OutgoingJingleFileTransfer::startTransferring(boost::shared_ptr<TransportSession> transportSession) {
	SWIFT_LOG(debug) << std::endl;

	this->transportSession = transportSession;
	processedBytesConnection = transportSession->onBytesSent.connect(
			boost::bind(boost::ref(onProcessedBytes), _1));
	transferFinishedConnection = transportSession->onFinished.connect(
			boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
	setState(Transferring);
	transportSession->start();
}


void OutgoingJingleFileTransfer::setState(State state) {
	SWIFT_LOG(debug) <<  state << std::endl;
	this->state = state;
	onStateChanged(FileTransfer::State(getExternalState(state)));
}

void OutgoingJingleFileTransfer::setFinishedState(
		FileTransfer::State::Type type, const boost::optional<FileTransferError>& error) {
	SWIFT_LOG(debug) << std::endl;
	this->state = Finished;
	onStateChanged(type);
	onFinished(error);
}

FileTransfer::State::Type OutgoingJingleFileTransfer::getExternalState(State state) {
	switch (state) {
		case Initial: return FileTransfer::State::Initial;
		case GeneratingInitialLocalCandidates: return FileTransfer::State::WaitingForStart;
		case WaitingForAccept: return FileTransfer::State::WaitingForAccept;
		case TryingCandidates: return FileTransfer::State::Negotiating;
		case WaitingForPeerProxyActivate: return FileTransfer::State::Negotiating;
		case WaitingForLocalProxyActivate: return FileTransfer::State::Negotiating;
		case WaitingForCandidateAcknowledge: return FileTransfer::State::Negotiating;
		case FallbackRequested: return FileTransfer::State::Negotiating;
		case Transferring: return FileTransfer::State::Transferring;
		case WaitForTermination: return FileTransfer::State::Transferring;
		case Finished: return FileTransfer::State::Finished;
	}
	assert(false);
	return FileTransfer::State::Initial;
}

void OutgoingJingleFileTransfer::stopAll() {
	SWIFT_LOG(debug) << state << std::endl;
	switch (state) {
		case Initial: SWIFT_LOG(warning) << "Not yet started" << std::endl; break;
		case GeneratingInitialLocalCandidates: transporter->stopGeneratingLocalCandidates(); break;
		case WaitingForAccept: break;
		case TryingCandidates: transporter->stopTryingRemoteCandidates(); break;
		case FallbackRequested: break;
		case WaitingForPeerProxyActivate: break;
		case WaitingForLocalProxyActivate: transporter->stopActivatingProxy(); break;
		case WaitingForCandidateAcknowledge: // Fallthrough
		case Transferring:
			assert(transportSession);
			processedBytesConnection.disconnect();
			transferFinishedConnection.disconnect();
			transportSession->stop();
			transportSession.reset();
			break;
		case WaitForTermination:
			break;
		case Finished: SWIFT_LOG(warning) << "Already finished" << std::endl; break;
	}
	if (state != Initial) {
		delete transporter;
	}
}

void OutgoingJingleFileTransfer::startTransferViaRemoteCandidate() {
	SWIFT_LOG(debug) << std::endl;

	if (ourCandidateChoice->type == JingleS5BTransportPayload::Candidate::ProxyType) {
		setState(WaitingForPeerProxyActivate);
	} 
	else {
		transportSession = createRemoteCandidateSession();
		startTransferringIfCandidateAcknowledged();
	}
}

void OutgoingJingleFileTransfer::startTransferViaLocalCandidate() {
	SWIFT_LOG(debug) << std::endl;

	if (theirCandidateChoice->type == JingleS5BTransportPayload::Candidate::ProxyType) {
		setState(WaitingForLocalProxyActivate);
		transporter->startActivatingProxy(theirCandidateChoice->jid);
	} 
	else {
		transportSession = createLocalCandidateSession();
		startTransferringIfCandidateAcknowledged();
	}
}

void OutgoingJingleFileTransfer::startTransferringIfCandidateAcknowledged() {
	if (candidateAcknowledged) {
		startTransferring(transportSession);
	}
	else {
		setState(WaitingForCandidateAcknowledge);
	}
}

void OutgoingJingleFileTransfer::handleTransportInfoAcknowledged(const std::string& id) {
	if (id == candidateSelectRequestID) {
		candidateAcknowledged = true;
	}
	if (state == WaitingForCandidateAcknowledge) {
		startTransferring(transportSession);
	}
}

JingleContentID OutgoingJingleFileTransfer::getContentID() const {
	return contentID;
}

bool OutgoingJingleFileTransfer::hasPriorityOnCandidateTie() const {
	return true;
}

bool OutgoingJingleFileTransfer::isWaitingForPeerProxyActivate() const {
	return state == WaitingForPeerProxyActivate;
}

bool OutgoingJingleFileTransfer::isWaitingForLocalProxyActivate() const {
	return state == WaitingForLocalProxyActivate;
}

bool OutgoingJingleFileTransfer::isTryingCandidates() const {
	return state == TryingCandidates;
}

boost::shared_ptr<TransportSession> OutgoingJingleFileTransfer::createLocalCandidateSession() {
	return transporter->createLocalCandidateSession(stream, theirCandidateChoice.get());
}

boost::shared_ptr<TransportSession> OutgoingJingleFileTransfer::createRemoteCandidateSession() {
	return transporter->createRemoteCandidateSession(stream, ourCandidateChoice.get());
}

void OutgoingJingleFileTransfer::handleWaitForRemoteTerminationTimeout() {
	assert(state == WaitForTermination);
	SWIFT_LOG(warning) << "Other party did not terminate session. Terminate it now." << std::endl;
	waitForRemoteTermination->stop();
	terminate(JinglePayload::Reason::MediaError);
}

