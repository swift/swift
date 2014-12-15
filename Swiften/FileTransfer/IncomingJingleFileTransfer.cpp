/*
 * Copyright (c) 2011-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Jingle/JingleSession.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>
#include <Swiften/FileTransfer/FileTransferTransporter.h>
#include <Swiften/FileTransfer/FileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/WriteBytestream.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/FileTransfer/TransportSession.h>

using namespace Swift;

// TODO: ALlow terminate when already terminated.

IncomingJingleFileTransfer::IncomingJingleFileTransfer(
		const JID& toJID,
		JingleSession::ref session,
		JingleContentPayload::ref content,
		FileTransferTransporterFactory* transporterFactory,
		TimerFactory* timerFactory,
		CryptoProvider* crypto) :
			JingleFileTransfer(session, toJID, transporterFactory),
			initialContent(content),
			crypto(crypto),
			state(Initial),
			receivedBytes(0),
			hashCalculator(NULL) {
	description = initialContent->getDescription<JingleFileTransferDescription>();
	assert(description);
	assert(description->getOffers().size() == 1);
	StreamInitiationFileInfo fileInfo = description->getOffers().front();
	setFileInfo(fileInfo.getName(), fileInfo.getSize());
	hash = fileInfo.getHash();
	hashAlgorithm = fileInfo.getAlgo();

	waitOnHashTimer = timerFactory->createTimer(5000);
	waitOnHashTimerTickedConnection = waitOnHashTimer->onTick.connect(
			boost::bind(&IncomingJingleFileTransfer::handleWaitOnHashTimerTicked, this));
}

IncomingJingleFileTransfer::~IncomingJingleFileTransfer() {
}

void IncomingJingleFileTransfer::accept(
		boost::shared_ptr<WriteBytestream> stream,
		const FileTransferOptions& options) {
	SWIFT_LOG(debug) << std::endl;
	if (state != Initial) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	assert(!this->stream);
	this->stream = stream;
	this->options = options;

	assert(!hashCalculator);
	hashCalculator = new IncrementalBytestreamHashCalculator(
			hashAlgorithm == "md5" || hash.empty(), hashAlgorithm == "sha-1" || hash.empty(), crypto);

	writeStreamDataReceivedConnection = stream->onWrite.connect(
			boost::bind(&IncomingJingleFileTransfer::handleWriteStreamDataReceived, this, _1));

	if (JingleS5BTransportPayload::ref s5bTransport = initialContent->getTransport<JingleS5BTransportPayload>()) {
		SWIFT_LOG(debug) << "Got S5B transport payload!" << std::endl;
		setTransporter(transporterFactory->createResponderTransporter(
				getInitiator(), getResponder(), s5bTransport->getSessionID()));
		transporter->addRemoteCandidates(s5bTransport->getCandidates());
		setState(GeneratingInitialLocalCandidates);
		transporter->startGeneratingLocalCandidates();
	}
	else {
		// Can't happen, because the transfer would have been rejected automatically
		assert(false);
	}
}

void IncomingJingleFileTransfer::cancel() {
	SWIFT_LOG(debug) << std::endl;
	terminate(state == Initial ? JinglePayload::Reason::Decline : JinglePayload::Reason::Cancel);
}

void IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated(
		const std::string& s5bSessionID, 
		const std::vector<JingleS5BTransportPayload::Candidate>& candidates) {
	SWIFT_LOG(debug) << std::endl;
	if (state != GeneratingInitialLocalCandidates) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	fillCandidateMap(localCandidates, candidates);

	JingleS5BTransportPayload::ref transport = boost::make_shared<JingleS5BTransportPayload>();
	transport->setSessionID(s5bSessionID);
	transport->setMode(JingleS5BTransportPayload::TCPMode);
	foreach(JingleS5BTransportPayload::Candidate candidate, candidates) {
		transport->addCandidate(candidate);	
	}
	session->sendAccept(getContentID(), initialContent->getDescriptions()[0], transport);

	setState(TryingCandidates);
	transporter->startTryingRemoteCandidates();
}


void IncomingJingleFileTransfer::handleSessionInfoReceived(JinglePayload::ref jinglePayload) {
	SWIFT_LOG(debug) << std::endl;

	JingleFileTransferHash::ref transferHash = jinglePayload->getPayload<JingleFileTransferHash>();
	if (transferHash) {
		SWIFT_LOG(debug) << "Received hash information." << std::endl;
		waitOnHashTimer->stop();
		if (transferHash->getHashes().find("sha-1") != transferHash->getHashes().end()) {
			hashAlgorithm = "sha-1";
			hash = transferHash->getHashes().find("sha-1")->second;
		}
		else if (transferHash->getHashes().find("md5") != transferHash->getHashes().end()) {
			hashAlgorithm = "md5";
			hash = transferHash->getHashes().find("md5")->second;
		}
		if (state == WaitingForHash) {
			checkHashAndTerminate();
		}
	}
	else {
		SWIFT_LOG(debug) << "Ignoring unknown session info" << std::endl;
	}
}

void IncomingJingleFileTransfer::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) {
	SWIFT_LOG(debug) << std::endl;
	if (state == Finished) { SWIFT_LOG(warning) << "Incorrect state" << std::endl; return; }

	if (state == Finished) { 
		SWIFT_LOG(debug) << "Already terminated" << std::endl;
		return; 
	}

	stopAll();
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

void IncomingJingleFileTransfer::checkHashAndTerminate() {
	if (verifyData()) {
		terminate(JinglePayload::Reason::Success);
	}
	else {
		SWIFT_LOG(warning) << "Hash verification failed" << std::endl;
		terminate(JinglePayload::Reason::MediaError);
	}
}

void IncomingJingleFileTransfer::checkIfAllDataReceived() {
	if (receivedBytes == getFileSizeInBytes()) {
		SWIFT_LOG(debug) << "All data received." << std::endl;
		if (hash.empty()) {
			SWIFT_LOG(debug) << "No hash information yet. Waiting a while on hash info." << std::endl;
			setState(WaitingForHash);
			waitOnHashTimer->start();
		} 
		else {
			checkHashAndTerminate();
		}
	}
	else if (receivedBytes > getFileSizeInBytes()) {
		SWIFT_LOG(debug) << "We got more than we could handle!" << std::endl;
		terminate(JinglePayload::Reason::MediaError);
	}
}

void IncomingJingleFileTransfer::handleWriteStreamDataReceived(
		const std::vector<unsigned char>& data) {
	hashCalculator->feedData(data);
	receivedBytes += data.size();
	checkIfAllDataReceived();
}

void IncomingJingleFileTransfer::handleTransportReplaceReceived(
		const JingleContentID& content, JingleTransportPayload::ref transport) {
	SWIFT_LOG(debug) << std::endl;
	if (state != WaitingForFallbackOrTerminate) { 
		SWIFT_LOG(warning) << "Incorrect state" << std::endl; 
		return; 
	}

	if (JingleIBBTransportPayload::ref ibbTransport = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport)) {
		SWIFT_LOG(debug) << "transport replaced with IBB" << std::endl;

		startTransferring(transporter->createIBBReceiveSession(
			ibbTransport->getSessionID(), 
			description->getOffers()[0].getSize(),
			stream));
		session->sendTransportAccept(content, ibbTransport);
	} 
	else {
		SWIFT_LOG(debug) << "Unknown replace transport" << std::endl;
		session->sendTransportReject(content, transport);
	}
}

JingleContentID IncomingJingleFileTransfer::getContentID() const {
	return JingleContentID(initialContent->getName(), initialContent->getCreator());
}

bool IncomingJingleFileTransfer::verifyData() {
	if (hashAlgorithm.empty() || hash.empty()) {
		SWIFT_LOG(debug) << "no verification possible, skipping" << std::endl;
		return true;
	} 
	if (hashAlgorithm == "sha-1") {
		SWIFT_LOG(debug) << "Verify SHA-1 hash: " << (hash == hashCalculator->getSHA1String()) << std::endl;
		return hash == hashCalculator->getSHA1String();
	}
	else if (hashAlgorithm == "md5") {
		SWIFT_LOG(debug) << "Verify MD5 hash: " << (hash == hashCalculator->getMD5String()) << std::endl;
		return hash == hashCalculator->getMD5String();
	}
	else {
		SWIFT_LOG(debug) << "Unknown hash, skipping" << std::endl;
		return true;
	}
}

void IncomingJingleFileTransfer::handleWaitOnHashTimerTicked() {
	SWIFT_LOG(debug) << std::endl;
	waitOnHashTimer->stop();
	terminate(JinglePayload::Reason::Success);
}

const JID& IncomingJingleFileTransfer::getSender() const {
	return getInitiator();
}

const JID& IncomingJingleFileTransfer::getRecipient() const {
	return getResponder();
}

void IncomingJingleFileTransfer::setState(State state) {
	SWIFT_LOG(debug) << state << std::endl;
	this->state = state;
	onStateChanged(FileTransfer::State(getExternalState(state)));
}

void IncomingJingleFileTransfer::setFinishedState(
		FileTransfer::State::Type type, const boost::optional<FileTransferError>& error) {
	SWIFT_LOG(debug) << std::endl;
	this->state = Finished;
	onStateChanged(type);
	onFinished(error);
}

void IncomingJingleFileTransfer::handleTransferFinished(boost::optional<FileTransferError> error) {
	if (error && state != WaitingForHash) {
		terminate(JinglePayload::Reason::MediaError);
	}
}

FileTransfer::State::Type IncomingJingleFileTransfer::getExternalState(State state) {
	switch (state) {
		case Initial: return FileTransfer::State::Initial;
		case GeneratingInitialLocalCandidates: return FileTransfer::State::WaitingForStart;
		case TryingCandidates: return FileTransfer::State::Negotiating;
		case WaitingForPeerProxyActivate: return FileTransfer::State::Negotiating;
		case WaitingForLocalProxyActivate: return FileTransfer::State::Negotiating;
		case WaitingForFallbackOrTerminate: return FileTransfer::State::Negotiating;
		case Transferring: return FileTransfer::State::Transferring;
		case WaitingForHash: return FileTransfer::State::Transferring;
		case Finished: return FileTransfer::State::Finished;
	}
	assert(false);
	return FileTransfer::State::Initial;
}

void IncomingJingleFileTransfer::stopAll() {
	if (state != Initial) {
		writeStreamDataReceivedConnection.disconnect();
		delete hashCalculator;
	}
	switch (state) {
		case Initial: break;
		case GeneratingInitialLocalCandidates: transporter->stopGeneratingLocalCandidates(); break;
		case TryingCandidates: transporter->stopTryingRemoteCandidates(); break;
		case WaitingForFallbackOrTerminate: break;
		case WaitingForPeerProxyActivate: break;
		case WaitingForLocalProxyActivate: transporter->stopActivatingProxy(); break;
		case WaitingForHash: // Fallthrough
		case Transferring:
			assert(transportSession);
			transferFinishedConnection.disconnect();
			transportSession->stop();
			transportSession.reset();
			break;
		case Finished: SWIFT_LOG(warning) << "Already finished" << std::endl; break;
	}
	if (state != Initial) {
		delete transporter;
	}
}

bool IncomingJingleFileTransfer::hasPriorityOnCandidateTie() const {
	return false;
}

void IncomingJingleFileTransfer::fallback() {
	if (options.isInBandAllowed()) {
		setState(WaitingForFallbackOrTerminate);
	}
	else {
		terminate(JinglePayload::Reason::ConnectivityError);
	}
}

void IncomingJingleFileTransfer::startTransferViaRemoteCandidate() {
	SWIFT_LOG(debug) << std::endl;

	if (ourCandidateChoice->type == JingleS5BTransportPayload::Candidate::ProxyType) {
		setState(WaitingForPeerProxyActivate);
	} 
	else {
		startTransferring(createRemoteCandidateSession());
	}
}

void IncomingJingleFileTransfer::startTransferViaLocalCandidate() {
	SWIFT_LOG(debug) << std::endl;

	if (theirCandidateChoice->type == JingleS5BTransportPayload::Candidate::ProxyType) {
		setState(WaitingForLocalProxyActivate);
		transporter->startActivatingProxy(theirCandidateChoice->jid);
	} 
	else {
		startTransferring(createLocalCandidateSession());
	}
}


void IncomingJingleFileTransfer::startTransferring(boost::shared_ptr<TransportSession> transportSession) {
	SWIFT_LOG(debug) << std::endl;

	this->transportSession = transportSession;
	transferFinishedConnection = transportSession->onFinished.connect(
			boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
	setState(Transferring);
	transportSession->start();
}

bool IncomingJingleFileTransfer::isWaitingForPeerProxyActivate() const {
	return state == WaitingForPeerProxyActivate;
}

bool IncomingJingleFileTransfer::isWaitingForLocalProxyActivate() const {
	return state == WaitingForLocalProxyActivate;
}

bool IncomingJingleFileTransfer::isTryingCandidates() const {
	return state == TryingCandidates;
}

boost::shared_ptr<TransportSession> IncomingJingleFileTransfer::createLocalCandidateSession() {
	return transporter->createLocalCandidateSession(stream);
}

boost::shared_ptr<TransportSession> IncomingJingleFileTransfer::createRemoteCandidateSession() {
	return transporter->createRemoteCandidateSession(stream);
}

void IncomingJingleFileTransfer::terminate(JinglePayload::Reason::Type reason) {
	SWIFT_LOG(debug) << reason << std::endl;

	if (state != Finished) {
		session->sendTerminate(reason);
	}
	stopAll();
	setFinishedState(getExternalFinishedState(reason), getFileTransferError(reason));
}
