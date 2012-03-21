/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/IncomingJingleFileTransfer.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/S5BProxyRequest.h>
#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>
#include <Swiften/FileTransfer/JingleIncomingIBBTransport.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {

IncomingJingleFileTransfer::IncomingJingleFileTransfer(
		const JID& ourJID,
		JingleSession::ref session,
		JingleContentPayload::ref content,
		RemoteJingleTransportCandidateSelectorFactory* candidateSelectorFactory,
		LocalJingleTransportCandidateGeneratorFactory* candidateGeneratorFactory,
		IQRouter* router,
		SOCKS5BytestreamRegistry* registry,
		SOCKS5BytestreamProxy* proxy,
		TimerFactory* timerFactory) :
			ourJID(ourJID),
			session(session),
			router(router),
			timerFactory(timerFactory),
			initialContent(content),
			state(Initial),
			receivedBytes(0),
			s5bRegistry(registry),
			s5bProxy(proxy),
			remoteTransportCandidateSelectFinished(false),
			localTransportCandidateSelectFinished(false),
			serverSession(0) {
	
	candidateSelector = candidateSelectorFactory->createCandidateSelector();
	candidateSelector->onRemoteTransportCandidateSelectFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished, this, _1));

	candidateGenerator = candidateGeneratorFactory->createCandidateGenerator();
	candidateGenerator->onLocalTransportCandidatesGenerated.connect(boost::bind(&IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated, this, _1));

	session->onTransportInfoReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportInfoReceived, this, _1, _2));
	session->onTransportReplaceReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportReplaceReceived, this, _1, _2));
	session->onSessionTerminateReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleSessionTerminateReceived, this, _1));
	session->onSessionInfoReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleSessionInfoReceived, this, _1));

	description = initialContent->getDescription<JingleFileTransferDescription>();
	assert(description);
	assert(description->getOffers().size() == 1);
	StreamInitiationFileInfo fileInfo = description->getOffers().front();
	fileSizeInBytes = fileInfo.getSize();
	filename = fileInfo.getName();
	hash = fileInfo.getHash();
	algo = fileInfo.getAlgo();

	waitOnHashTimer = timerFactory->createTimer(5000);
	waitOnHashTimer->onTick.connect(boost::bind(&IncomingJingleFileTransfer::finishOffTransfer, this));
}

IncomingJingleFileTransfer::~IncomingJingleFileTransfer() {
	stream->onWrite.disconnect(boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));
	delete hashCalculator;

	session->onSessionTerminateReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleSessionTerminateReceived, this, _1));
	session->onTransportReplaceReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportReplaceReceived, this, _1, _2));
	session->onTransportInfoReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportInfoReceived, this, _1, _2));

	candidateGenerator->onLocalTransportCandidatesGenerated.disconnect(boost::bind(&IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated, this, _1));
	delete candidateGenerator;

	candidateSelector->onRemoteTransportCandidateSelectFinished.disconnect(boost::bind(&IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished, this, _1));
	delete candidateSelector;
}

void IncomingJingleFileTransfer::accept(WriteBytestream::ref stream) {
	assert(!this->stream);
	this->stream = stream;

	hashCalculator = new IncrementalBytestreamHashCalculator( algo == "md5" || hash.empty() , algo == "sha-1" || hash.empty() );
	stream->onWrite.connect(boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));
	stream->onWrite.connect(boost::bind(&IncomingJingleFileTransfer::handleWriteStreamDataReceived, this, _1));
	onStateChange(FileTransfer::State(FileTransfer::State::Negotiating));
	if (JingleIBBTransportPayload::ref ibbTransport = initialContent->getTransport<JingleIBBTransportPayload>()) {
		SWIFT_LOG(debug) << "Got IBB transport payload!" << std::endl;
		setActiveTransport(createIBBTransport(ibbTransport));
		session->sendAccept(getContentID(), initialContent->getDescriptions()[0], ibbTransport);
	}
	else if (JingleS5BTransportPayload::ref s5bTransport = initialContent->getTransport<JingleS5BTransportPayload>()) {
		SWIFT_LOG(debug) << "Got S5B transport payload!" << std::endl;
		state = CreatingInitialTransports;
		s5bSessionID = s5bTransport->getSessionID().empty() ? idGenerator.generateID() : s5bTransport->getSessionID();
		s5bDestination = SOCKS5BytestreamRegistry::getHostname(s5bSessionID, ourJID, session->getInitiator());
		s5bRegistry->addWriteBytestream(s5bDestination, stream);
		fillCandidateMap(theirCandidates, s5bTransport);
		candidateSelector->addRemoteTransportCandidates(s5bTransport);
		candidateSelector->setRequesterTargtet(session->getInitiator(), ourJID);
		s5bTransport->setSessionID(s5bSessionID);
		candidateGenerator->generateLocalTransportCandidates(s5bTransport);
	}
	else {
		assert(false);
	}
}

const JID& IncomingJingleFileTransfer::getSender() const {
	return session->getInitiator();
}

const JID& IncomingJingleFileTransfer::getRecipient() const {
	return ourJID;
}

void IncomingJingleFileTransfer::cancel() {
	session->sendTerminate(JinglePayload::Reason::Cancel);

	if (activeTransport) activeTransport->stop();
	if (serverSession) serverSession->stop();
	if (clientSession) clientSession->stop();
	onStateChange(FileTransfer::State(FileTransfer::State::Canceled));
}

void IncomingJingleFileTransfer::handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref candidates) {
	if (state == CreatingInitialTransports) {
		if (JingleS5BTransportPayload::ref s5bCandidates = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(candidates)) {
			//localTransportCandidateSelectFinished = true;
			//JingleS5BTransportPayload::ref emptyCandidates = boost::make_shared<JingleS5BTransportPayload>();
			//emptyCandidates->setSessionID(s5bCandidates->getSessionID());
			fillCandidateMap(ourCandidates, s5bCandidates);
			session->sendAccept(getContentID(), initialContent->getDescriptions()[0], s5bCandidates);

			state = NegotiatingTransport;
			candidateSelector->selectCandidate();
		}
	}
	else {
		SWIFT_LOG(debug) << "Unhandled state!" << std::endl;
	}
}


void IncomingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref transport) {
	SWIFT_LOG(debug) << std::endl;
	if (state == Terminated) {
		return;
	}
	if (JingleS5BTransportPayload::ref s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transport)) {
		//remoteTransportCandidateSelectFinished = true;
		//selectedRemoteTransportCandidate = transport;
		ourCandidate = s5bPayload;
		//checkCandidateSelected();
		decideOnUsedTransport();
		session->sendTransportInfo(getContentID(), s5bPayload);
	}
	else {
		SWIFT_LOG(debug) << "Expected something different here." << std::endl;
	}
}

void IncomingJingleFileTransfer::checkCandidateSelected() {
	assert(false);
	if (localTransportCandidateSelectFinished && remoteTransportCandidateSelectFinished) {
		if (candidateGenerator->isActualCandidate(selectedLocalTransportCandidate) && candidateSelector->isActualCandidate(selectedRemoteTransportCandidate)) {
			if (candidateGenerator->getPriority(selectedLocalTransportCandidate) > candidateSelector->getPriority(selectedRemoteTransportCandidate)) {
				setActiveTransport(candidateGenerator->selectTransport(selectedLocalTransportCandidate));
			}
			else {
				setActiveTransport(candidateSelector->selectTransport(selectedRemoteTransportCandidate));
			}
		}
		else if (candidateSelector->isActualCandidate(selectedRemoteTransportCandidate)) {
			setActiveTransport(candidateSelector->selectTransport(selectedRemoteTransportCandidate));
		}
		else if (candidateGenerator->isActualCandidate(selectedLocalTransportCandidate)) {
			setActiveTransport(candidateGenerator->selectTransport(selectedLocalTransportCandidate));
		}
		else {
			state = WaitingForFallbackOrTerminate;
		}
	}
}

void IncomingJingleFileTransfer::setActiveTransport(JingleTransport::ref transport) {
	state = Transferring;
	onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	activeTransport = transport;
	activeTransport->onDataReceived.connect(boost::bind(&IncomingJingleFileTransfer::handleTransportDataReceived, this, _1));
	activeTransport->onFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
	activeTransport->start();
}

bool IncomingJingleFileTransfer::verifyReceviedData() {
	if (algo.empty() || hash.empty()) {
		SWIFT_LOG(debug) << "no verification possible, skipping" << std::endl;
		return true;
	} else {
		if (algo == "sha-1") {
			SWIFT_LOG(debug) << "verify data via SHA-1 hash: " << (hash == hashCalculator->getSHA1String()) << std::endl;
			return hash == hashCalculator->getSHA1String();
		}
		else if (algo == "md5") {
			SWIFT_LOG(debug) << "verify data via MD5 hash: " << (hash == hashCalculator->getMD5String()) << std::endl;
			return hash == hashCalculator->getMD5String();
		}
		else {
			SWIFT_LOG(debug) << "no verification possible, skipping" << std::endl;
			return true;
		}
	}
}

void IncomingJingleFileTransfer::finishOffTransfer() {
	if (verifyReceviedData()) {
		onStateChange(FileTransfer::State(FileTransfer::State::Finished));
		session->sendTerminate(JinglePayload::Reason::Success);
	} else {
		onStateChange(FileTransfer::State(FileTransfer::State::Failed, "Verification failed."));
		session->sendTerminate(JinglePayload::Reason::MediaError);
	}
	state = Terminated;
	waitOnHashTimer->stop();
}

void IncomingJingleFileTransfer::handleSessionInfoReceived(JinglePayload::ref jinglePayload) {
	if (state == Terminated) {
		return;
	}
	JingleFileTransferHash::ref transferHash = jinglePayload->getPayload<JingleFileTransferHash>();
	if (transferHash) {
		SWIFT_LOG(debug) << "Recevied hash information." << std::endl;
		if (transferHash->getHashes().find("sha-1") != transferHash->getHashes().end()) {
			algo = "sha-1";
			hash = transferHash->getHashes().find("sha-1")->second;
		}
		else if (transferHash->getHashes().find("md5") != transferHash->getHashes().end()) {
			algo = "md5";
			hash = transferHash->getHashes().find("md5")->second;
		}
		checkIfAllDataReceived();
	}
}

void IncomingJingleFileTransfer::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) {
	SWIFT_LOG(debug) << "session terminate received" << std::endl;
	if (activeTransport) activeTransport->stop();
	if (reason && reason.get().type == JinglePayload::Reason::Cancel) {
		onStateChange(FileTransfer::State(FileTransfer::State::Canceled, "Other user canceled the transfer."));
	}
	else if (reason && reason.get().type == JinglePayload::Reason::Success) {
		/*if (verifyReceviedData()) {
			onStateChange(FileTransfer::State(FileTransfer::State::Finished));
		} else {
			onStateChange(FileTransfer::State(FileTransfer::State::Failed, "Verification failed."));
		}*/
	}
	state = Terminated;
}

void IncomingJingleFileTransfer::checkIfAllDataReceived() {
	if (receivedBytes == fileSizeInBytes) {
		SWIFT_LOG(debug) << "All data received." << std::endl;
		if (hash.empty()) {
			SWIFT_LOG(debug) << "No hash information yet. Waiting 5 seconds on hash info." << std::endl;
			waitOnHashTimer->start();
		} else {
			SWIFT_LOG(debug) << "We already have hash info using " << algo << " algorithm. Finishing off transfer." << std::endl;
			finishOffTransfer();
		}
	}
	else if (receivedBytes > fileSizeInBytes) {
		SWIFT_LOG(debug) << "We got more than we could handle!" << std::endl;
	}
}

void IncomingJingleFileTransfer::handleTransportDataReceived(const std::vector<unsigned char>& data) {
	SWIFT_LOG(debug) << data.size() << " bytes received" << std::endl;
	onProcessedBytes(data.size());
	stream->write(data);
	receivedBytes += data.size();
	checkIfAllDataReceived();
}

void IncomingJingleFileTransfer::handleWriteStreamDataReceived(const std::vector<unsigned char>& data) {
	receivedBytes += data.size();
	checkIfAllDataReceived();
}

void IncomingJingleFileTransfer::useOurCandidateChoiceForTransfer(JingleS5BTransportPayload::Candidate candidate) {
	SWIFT_LOG(debug) << std::endl;
	if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
		// get proxy client session from remoteCandidateSelector
		clientSession = candidateSelector->getS5BSession();

		// wait on <activated/> transport-info
	} else {
		// ask s5b client
		clientSession = candidateSelector->getS5BSession();
		if (clientSession) {
			state = Transferring;
			SWIFT_LOG(debug) << clientSession->getAddressPort().toString() << std::endl;
			clientSession->onBytesReceived.connect(boost::bind(boost::ref(onProcessedBytes), _1));
			clientSession->onFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
			clientSession->startReceiving(stream);
		} else {
			SWIFT_LOG(debug) << "No S5B client session found!!!" << std::endl;
		}
	}
}

void IncomingJingleFileTransfer::useTheirCandidateChoiceForTransfer(JingleS5BTransportPayload::Candidate candidate) {
	SWIFT_LOG(debug) << std::endl;

	if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
		// get proxy client session from s5bRegistry
		clientSession = s5bProxy->createSOCKS5BytestreamClientSession(candidate.hostPort, SOCKS5BytestreamRegistry::getHostname(s5bSessionID, ourJID, session->getInitiator()));
		clientSession->onSessionReady.connect(boost::bind(&IncomingJingleFileTransfer::proxySessionReady, this, candidate.jid, _1));
		clientSession->start();

		// on reply send activate
	} else {
		// ask s5b server
		serverSession = s5bRegistry->getConnectedSession(s5bDestination);
		if (serverSession) {
			state = Transferring;
			serverSession->onBytesReceived.connect(boost::bind(boost::ref(onProcessedBytes), _1));
			serverSession->onFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
			serverSession->startTransfer();
		} else {
			SWIFT_LOG(debug) << "No S5B server session found!!!" << std::endl;
		}
	}
}

void IncomingJingleFileTransfer::fillCandidateMap(CandidateMap& map, JingleS5BTransportPayload::ref s5bPayload) {
	map.clear();
	foreach (JingleS5BTransportPayload::Candidate candidate, s5bPayload->getCandidates()) {
		map[candidate.cid] = candidate;
	}
}


void IncomingJingleFileTransfer::decideOnUsedTransport() {
	if (ourCandidate && theirCandidate) {
		if (ourCandidate->hasCandidateError() && theirCandidate->hasCandidateError()) {
			state = WaitingForFallbackOrTerminate;
			return;
		}
		std::string our_cid = ourCandidate->getCandidateUsed();
		std::string their_cid = theirCandidate->getCandidateUsed();
		if (ourCandidate->hasCandidateError() && !their_cid.empty()) {
			useTheirCandidateChoiceForTransfer(ourCandidates[their_cid]);
			onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
		}
		else if (theirCandidate->hasCandidateError() && !our_cid.empty()) {
			useOurCandidateChoiceForTransfer(theirCandidates[our_cid]);
			onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
		}
		else if (!our_cid.empty() && !their_cid.empty()) {
			// compare priorites, if same they win
			if (ourCandidates.find(their_cid) == ourCandidates.end() || theirCandidates.find(our_cid) == theirCandidates.end()) {
				SWIFT_LOG(debug) << "Didn't recognize candidate IDs!" << std::endl;
				session->sendTerminate(JinglePayload::Reason::FailedTransport);
				onStateChange(FileTransfer::State(FileTransfer::State::Canceled, "Failed to negotiate candidate."));
				onFinished(FileTransferError(FileTransferError::PeerError));
				return;
			}

			JingleS5BTransportPayload::Candidate our_candidate = theirCandidates[our_cid];
			JingleS5BTransportPayload::Candidate their_candidate = ourCandidates[their_cid];
			if (our_candidate.priority > their_candidate.priority) {
				useOurCandidateChoiceForTransfer(our_candidate);
			}
			else if (our_candidate.priority < their_candidate.priority) {
				useTheirCandidateChoiceForTransfer(their_candidate);
			}
			else {
				useTheirCandidateChoiceForTransfer(their_candidate);
			}
			onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
		}
		else {
			assert(false);
		}
	} else {
		SWIFT_LOG(debug) << "Can't make a transport decision yet." << std::endl;
	}
}

void IncomingJingleFileTransfer::proxySessionReady(const JID& proxy, bool error) {
	if (error) {
		// indicate proxy error
	} else {
		// activate proxy
		activateProxySession(proxy);
	}
}

void IncomingJingleFileTransfer::activateProxySession(const JID &proxy) {
	S5BProxyRequest::ref proxyRequest = boost::make_shared<S5BProxyRequest>();
	proxyRequest->setSID(s5bSessionID);
	proxyRequest->setActivate(session->getInitiator());

	boost::shared_ptr<GenericRequest<S5BProxyRequest> > request = boost::make_shared<GenericRequest<S5BProxyRequest> >(IQ::Set, proxy, proxyRequest, router);
	request->onResponse.connect(boost::bind(&IncomingJingleFileTransfer::handleActivateProxySessionResult, this, _1, _2));
	request->send();
}

void IncomingJingleFileTransfer::handleActivateProxySessionResult(boost::shared_ptr<S5BProxyRequest> /*request*/, ErrorPayload::ref error) {
	SWIFT_LOG(debug) << std::endl;
	if (error) {
		SWIFT_LOG(debug) << "ERROR" << std::endl;
	} else {
		// send activated to other jingle party
		JingleS5BTransportPayload::ref proxyActivate = boost::make_shared<JingleS5BTransportPayload>();
		proxyActivate->setActivated(theirCandidate->getCandidateUsed());
		session->sendTransportInfo(getContentID(), proxyActivate);

		// start transferring
		clientSession->onBytesReceived.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		clientSession->onFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
		clientSession->startReceiving(stream);
		onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	}
}

void IncomingJingleFileTransfer::handleTransportInfoReceived(const JingleContentID&, JingleTransportPayload::ref transport) {
	SWIFT_LOG(debug) << "transport info received" << std::endl;
	if (state == Terminated) {
		return;
	}
	if (JingleS5BTransportPayload::ref s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transport)) {
		if (!s5bPayload->getActivated().empty()) {
			if (ourCandidate->getCandidateUsed() == s5bPayload->getActivated()) {
				clientSession->onBytesReceived.connect(boost::bind(boost::ref(onProcessedBytes), _1));
				clientSession->onFinished.connect(boost::bind(&IncomingJingleFileTransfer::handleTransferFinished, this, _1));
				clientSession->startReceiving(stream);
				onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
			} else {
				SWIFT_LOG(debug) << "ourCandidateChoice doesn't match activated proxy candidate!" << std::endl;
				JingleS5BTransportPayload::ref proxyError = boost::make_shared<JingleS5BTransportPayload>();
				proxyError->setProxyError(true);
				proxyError->setSessionID(s5bSessionID);
				session->sendTransportInfo(getContentID(), proxyError);
			}
		} else {
			theirCandidate = s5bPayload;
			decideOnUsedTransport();
		}
	}
	else {
		SWIFT_LOG(debug) << "Expected something different here." << std::endl;
	}
	/*localTransportCandidateSelectFinished = true;
	selectedLocalTransportCandidate = transport;
	if (candidateGenerator->isActualCandidate(transport)) {
		candidateSelector->setMinimumPriority(candidateGenerator->getPriority(transport));
	}*/
	//checkCandidateSelected();
}

void IncomingJingleFileTransfer::handleTransportReplaceReceived(const JingleContentID& content, JingleTransportPayload::ref transport) {
	if (state == Terminated) {
		return;
	}
	if (JingleIBBTransportPayload::ref ibbTransport = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport)) {
		SWIFT_LOG(debug) << "transport replaced with IBB" << std::endl;
		setActiveTransport(createIBBTransport(ibbTransport));
		session->sendTransportAccept(content, ibbTransport);
	} else {
		SWIFT_LOG(debug) << "transport replaced failed" << std::endl;
		session->sendTransportReject(content, transport);
	}
}

void IncomingJingleFileTransfer::stopActiveTransport() {
	if (activeTransport) {
		activeTransport->stop();
		activeTransport->onDataReceived.disconnect(boost::bind(&IncomingJingleFileTransfer::handleTransportDataReceived, this, _1));
	}
}

JingleIncomingIBBTransport::ref IncomingJingleFileTransfer::createIBBTransport(JingleIBBTransportPayload::ref ibbTransport) {
	// TODO: getOffer() -> getOffers correction
	return boost::make_shared<JingleIncomingIBBTransport>(session->getInitiator(), getRecipient(), ibbTransport->getSessionID(), description->getOffers()[0].getSize(), router);
}

JingleContentID IncomingJingleFileTransfer::getContentID() const {
	return JingleContentID(initialContent->getName(), initialContent->getCreator());
}

void IncomingJingleFileTransfer::handleTransferFinished(boost::optional<FileTransferError> error) {
	if (state == Terminated) {
		return;
	}

	if (error) {
		session->sendTerminate(JinglePayload::Reason::ConnectivityError);
		onStateChange(FileTransfer::State(FileTransfer::State::Failed));
		onFinished(error);
	}
	//
}

}
