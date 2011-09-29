/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "OutgoingJingleFileTransfer.h"

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Jingle/JingleContentID.h>
#include <Swiften/Elements/JingleFileTransferDescription.h>
#include <Swiften/Elements/JingleFileTransferHash.h>
#include <Swiften/Elements/JingleTransportPayload.h>
#include <Swiften/Elements/JingleIBBTransportPayload.h>
#include <Swiften/Elements/JingleS5BTransportPayload.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/IncrementalBytestreamHashCalculator.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGeneratorFactory.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelectorFactory.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxy.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/StringCodecs/SHA1.h>

#include <Swiften/Base/Log.h>

namespace Swift {

OutgoingJingleFileTransfer::OutgoingJingleFileTransfer(JingleSession::ref session,
					RemoteJingleTransportCandidateSelectorFactory* remoteFactory,
					LocalJingleTransportCandidateGeneratorFactory* localFactory,
					IQRouter* router,
					IDGenerator *idGenerator,
					const JID& fromJID,
					const JID& toJID,
					boost::shared_ptr<ReadBytestream> readStream,
					const StreamInitiationFileInfo &fileInfo,
					SOCKS5BytestreamRegistry* bytestreamRegistry,
					SOCKS5BytestreamProxy* bytestreamProxy) :
	session(session), remoteFactory(remoteFactory), localFactory(localFactory), router(router), idGenerator(idGenerator), fromJID(fromJID), toJID(toJID), readStream(readStream), fileInfo(fileInfo), s5bRegistry(bytestreamRegistry), s5bProxy(bytestreamProxy), serverSession(NULL), contentID(JingleContentID(idGenerator->generateID(), JingleContentPayload::InitiatorCreator)), canceled(false) {
	session->onSessionAcceptReceived.connect(boost::bind(&OutgoingJingleFileTransfer::handleSessionAcceptReceived, this, _1, _2, _3));
	session->onSessionTerminateReceived.connect(boost::bind(&OutgoingJingleFileTransfer::handleSessionTerminateReceived, this, _1));
	session->onTransportInfoReceived.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransportInfoReceived, this, _1, _2));
	session->onTransportAcceptReceived.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransportAcceptReceived, this, _1, _2));
	fileSizeInBytes = fileInfo.getSize();
	filename = fileInfo.getName();

	localCandidateGenerator = localFactory->createCandidateGenerator();
	localCandidateGenerator->onLocalTransportCandidatesGenerated.connect(boost::bind(&OutgoingJingleFileTransfer::handleLocalTransportCandidatesGenerated, this, _1));

	remoteCandidateSelector = remoteFactory->createCandidateSelector();
	remoteCandidateSelector->onRemoteTransportCandidateSelectFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished, this, _1));
	// calculate both, MD5 and SHA-1 since we don't know which one the other side supports
	hashCalculator = new IncrementalBytestreamHashCalculator(true, true);
	this->readStream->onRead.connect(boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));
}

OutgoingJingleFileTransfer::~OutgoingJingleFileTransfer() {
	readStream->onRead.disconnect(boost::bind(&IncrementalBytestreamHashCalculator::feedData, hashCalculator, _1));
	delete hashCalculator;
}
	
void OutgoingJingleFileTransfer::start() {
	onStateChange(FileTransfer::State(FileTransfer::State::WaitingForStart));

	s5bSessionID = s5bRegistry->generateSessionID();
	SWIFT_LOG(debug) << "S5B SessionID: " << s5bSessionID << std::endl;

	//s5bProxy->connectToProxies(s5bSessionID);

	JingleS5BTransportPayload::ref transport = boost::make_shared<JingleS5BTransportPayload>();
	localCandidateGenerator->generateLocalTransportCandidates(transport);
}

void OutgoingJingleFileTransfer::stop() {

}

void OutgoingJingleFileTransfer::cancel() {
	canceled = true;
	session->sendTerminate(JinglePayload::Reason::Cancel);

	if (ibbSession) {
		ibbSession->stop();
	}
	SOCKS5BytestreamServerSession *serverSession = s5bRegistry->getConnectedSession(SOCKS5BytestreamRegistry::getHostname(s5bSessionID, session->getInitiator(), toJID));
	if (serverSession) {
		serverSession->stop();
	}
	if (clientSession) {
		clientSession->stop();
	}
	onStateChange(FileTransfer::State(FileTransfer::State::Canceled));
}

void OutgoingJingleFileTransfer::handleSessionAcceptReceived(const JingleContentID& id, JingleDescription::ref /* decription */, JingleTransportPayload::ref transportPayload) {
	if (canceled) {
		return;
	}
	onStateChange(FileTransfer::State(FileTransfer::State::Negotiating));

	JingleIBBTransportPayload::ref ibbPayload;
	JingleS5BTransportPayload::ref s5bPayload;
	if ((ibbPayload = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transportPayload))) {
		ibbSession = boost::make_shared<IBBSendSession>(ibbPayload->getSessionID(), fromJID, toJID, readStream, router);
		ibbSession->setBlockSize(ibbPayload->getBlockSize());
		ibbSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		ibbSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
		ibbSession->start();
		onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	}
	else if ((s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transportPayload))) {
		fillCandidateMap(theirCandidates, s5bPayload);
		remoteCandidateSelector->setRequesterTargtet(toJID, session->getInitiator());
		remoteCandidateSelector->addRemoteTransportCandidates(s5bPayload);
		remoteCandidateSelector->selectCandidate();
	}
	else {
		// TODO: error handling
		SWIFT_LOG(debug) << "Unknown transport payload! Try replaceing with IBB." << std::endl;
		replaceTransportWithIBB(id);
	}
}

void OutgoingJingleFileTransfer::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason> reason) {
	if (canceled) {
		return;
	}

	if (ibbSession) {
		ibbSession->stop();
	}
	if (clientSession) {
		clientSession->stop();
	}
	if (serverSession) {
		serverSession->stop();
	}

	if (reason.is_initialized() && reason.get().type == JinglePayload::Reason::Cancel) {
		onStateChange(FileTransfer::State(FileTransfer::State::Canceled));
		onFinished(FileTransferError(FileTransferError::PeerError));
	} else if (reason.is_initialized() && reason.get().type == JinglePayload::Reason::Success) {
		onStateChange(FileTransfer::State(FileTransfer::State::Finished));
		onFinished(boost::optional<FileTransferError>());
	} else {
		onStateChange(FileTransfer::State(FileTransfer::State::Failed));
		onFinished(FileTransferError(FileTransferError::PeerError));
	}
	canceled = true;
}

void OutgoingJingleFileTransfer::handleTransportAcceptReceived(const JingleContentID& /* contentID */, JingleTransportPayload::ref transport) {
	if (canceled) {
		return;
	}

	if (JingleIBBTransportPayload::ref ibbPayload = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(transport)) {
		ibbSession = boost::make_shared<IBBSendSession>(ibbPayload->getSessionID(), fromJID, toJID, readStream, router);
		ibbSession->setBlockSize(ibbPayload->getBlockSize());
		ibbSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		ibbSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
		ibbSession->start();
		onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	} else {
		// error handling
		SWIFT_LOG(debug) << "Replacing with anything other than IBB isn't supported yet." << std::endl;
		session->sendTerminate(JinglePayload::Reason::FailedTransport);
		onStateChange(FileTransfer::State(FileTransfer::State::Failed));
	}
}

void OutgoingJingleFileTransfer::startTransferViaOurCandidateChoice(JingleS5BTransportPayload::Candidate candidate) {
	SWIFT_LOG(debug) << "Transferring data using our candidate." << std::endl;
	if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
		// get proxy client session from remoteCandidateSelector
		clientSession = remoteCandidateSelector->getS5BSession();

		// wait on <activated/> transport-info
	} else {
		clientSession = remoteCandidateSelector->getS5BSession();
		clientSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		clientSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
		clientSession->startSending(readStream);
		onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	}
	assert(clientSession);
}

void OutgoingJingleFileTransfer::startTransferViaTheirCandidateChoice(JingleS5BTransportPayload::Candidate candidate) {
	SWIFT_LOG(debug) << "Transferring data using their candidate." << std::endl;
	if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
		// connect to proxy
		clientSession = s5bProxy->createSOCKS5BytestreamClientSession(candidate.hostPort, SOCKS5BytestreamRegistry::getHostname(s5bSessionID, session->getInitiator(), toJID));
		clientSession->onSessionReady.connect(boost::bind(&OutgoingJingleFileTransfer::proxySessionReady, this, candidate.jid, _1));
		clientSession->start();

		// on reply send activate

	} else {
		serverSession = s5bRegistry->getConnectedSession(SOCKS5BytestreamRegistry::getHostname(s5bSessionID, session->getInitiator(), toJID));
		serverSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		serverSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
		serverSession->startTransfer();
	}
	onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
}

// decide on candidates according to http://xmpp.org/extensions/xep-0260.html#complete
void OutgoingJingleFileTransfer::decideOnCandidates() {
	if (ourCandidateChoice && theirCandidateChoice) {
		std::string our_cid = ourCandidateChoice->getCandidateUsed();
		std::string their_cid = theirCandidateChoice->getCandidateUsed();
		if (ourCandidateChoice->hasCandidateError() && theirCandidateChoice->hasCandidateError()) {
			replaceTransportWithIBB(contentID);
		}
		else if (!our_cid.empty() && theirCandidateChoice->hasCandidateError()) {
			// use our candidate
			startTransferViaOurCandidateChoice(theirCandidates[our_cid]);
		}
		else if (!their_cid.empty() && ourCandidateChoice->hasCandidateError()) {
			// use their candidate
			startTransferViaTheirCandidateChoice(ourCandidates[their_cid]);
		}
		else if (!our_cid.empty() && !their_cid.empty()) {
			// compare priorites, if same we win
			if (ourCandidates.find(their_cid) == ourCandidates.end() || theirCandidates.find(our_cid) == theirCandidates.end()) {
				SWIFT_LOG(debug) << "Didn't recognize candidate IDs!" << std::endl;
				session->sendTerminate(JinglePayload::Reason::FailedTransport);
				onStateChange(FileTransfer::State(FileTransfer::State::Failed));
				onFinished(FileTransferError(FileTransferError::PeerError));
				return;
			}

			JingleS5BTransportPayload::Candidate ourCandidate = theirCandidates[our_cid];
			JingleS5BTransportPayload::Candidate theirCandidate = ourCandidates[their_cid];
			if (ourCandidate.priority > theirCandidate.priority) {
				startTransferViaOurCandidateChoice(ourCandidate);
			}
			else if (ourCandidate.priority < theirCandidate.priority) {
				startTransferViaTheirCandidateChoice(theirCandidate);
			}
			else {
				startTransferViaOurCandidateChoice(ourCandidate);
			}
		}
	} else {
		SWIFT_LOG(debug) << "Can't make a decision yet!" << std::endl;
	}
}

void OutgoingJingleFileTransfer::fillCandidateMap(CandidateMap& map, JingleS5BTransportPayload::ref s5bPayload) {
	map.clear();
	foreach (JingleS5BTransportPayload::Candidate candidate, s5bPayload->getCandidates()) {
		map[candidate.cid] = candidate;
	}
}

void OutgoingJingleFileTransfer::proxySessionReady(const JID& proxy, bool error) {
	if (error) {
		// indicate proxy error
	} else {
		// activate proxy
		activateProxySession(proxy);
	}
}

void OutgoingJingleFileTransfer::activateProxySession(const JID& proxy) {
	S5BProxyRequest::ref proxyRequest = boost::make_shared<S5BProxyRequest>();
	proxyRequest->setSID(s5bSessionID);
	proxyRequest->setActivate(toJID);

	boost::shared_ptr<GenericRequest<S5BProxyRequest> > request = boost::make_shared<GenericRequest<S5BProxyRequest> >(IQ::Set, proxy, proxyRequest, router);
	request->onResponse.connect(boost::bind(&OutgoingJingleFileTransfer::handleActivateProxySessionResult, this, _1, _2));
	request->send();
}

void OutgoingJingleFileTransfer::handleActivateProxySessionResult(boost::shared_ptr<S5BProxyRequest> /*request*/, ErrorPayload::ref error) {
	if (error) {
		SWIFT_LOG(debug) << "ERROR" << std::endl;
	} else {
		// send activated to other jingle party
		JingleS5BTransportPayload::ref proxyActivate = boost::make_shared<JingleS5BTransportPayload>();
		proxyActivate->setActivated(theirCandidateChoice->getCandidateUsed());
		session->sendTransportInfo(contentID, proxyActivate);

		// start transferring
		clientSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
		clientSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
		clientSession->startSending(readStream);
		onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
	}
}

void OutgoingJingleFileTransfer::sendSessionInfoHash() {
	SWIFT_LOG(debug) << std::endl;
	JingleFileTransferHash::ref hashElement = boost::make_shared<JingleFileTransferHash>();
	hashElement->setHash("sha-1", hashCalculator->getSHA1String());
	hashElement->setHash("md5", hashCalculator->getMD5String());
	session->sendInfo(hashElement);
}

void OutgoingJingleFileTransfer::handleTransportInfoReceived(const JingleContentID& /* contentID */, JingleTransportPayload::ref transport) {
	if (canceled) {
		return;
	}
	if (JingleS5BTransportPayload::ref s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(transport)) {
		if (s5bPayload->hasCandidateError() || !s5bPayload->getCandidateUsed().empty()) {
			theirCandidateChoice = s5bPayload;
			decideOnCandidates();
		} else if(!s5bPayload->getActivated().empty()) {
			if (ourCandidateChoice->getCandidateUsed() == s5bPayload->getActivated()) {
				clientSession->onBytesSent.connect(boost::bind(boost::ref(onProcessedBytes), _1));
				clientSession->onFinished.connect(boost::bind(&OutgoingJingleFileTransfer::handleTransferFinished, this, _1));
				clientSession->startSending(readStream);
				onStateChange(FileTransfer::State(FileTransfer::State::Transferring));
			} else {
				SWIFT_LOG(debug) << "ourCandidateChoice doesn't match activated proxy candidate!" << std::endl;
				JingleS5BTransportPayload::ref proxyError = boost::make_shared<JingleS5BTransportPayload>();
				proxyError->setProxyError(true);
				proxyError->setSessionID(s5bSessionID);
				session->sendTransportInfo(contentID, proxyError);
			}
		}
	}
}

void OutgoingJingleFileTransfer::handleLocalTransportCandidatesGenerated(JingleTransportPayload::ref payload) {
	if (canceled) {
		return;
	}
	JingleFileTransferDescription::ref description = boost::make_shared<JingleFileTransferDescription>();
	description->addOffer(fileInfo);

	JingleTransportPayload::ref transport;
	if (JingleIBBTransportPayload::ref ibbTransport = boost::dynamic_pointer_cast<JingleIBBTransportPayload>(payload)) {
		ibbTransport->setBlockSize(4096);
		ibbTransport->setSessionID(idGenerator->generateID());
		transport = ibbTransport;
	}
	else if (JingleS5BTransportPayload::ref s5bTransport = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(payload)) {
		//fillCandidateMap(ourCandidates, s5bTransport);
		//s5bTransport->setSessionID(s5bSessionID);

		JingleS5BTransportPayload::ref emptyCandidates = boost::make_shared<JingleS5BTransportPayload>();
		emptyCandidates->setSessionID(s5bTransport->getSessionID());
		fillCandidateMap(ourCandidates, emptyCandidates);

		transport = emptyCandidates;
		s5bRegistry->addReadBytestream(SOCKS5BytestreamRegistry::getHostname(s5bSessionID, session->getInitiator(), toJID), readStream);
	}
	else {
		SWIFT_LOG(debug) << "Unknown tranport payload: " << typeid(*payload).name() << std::endl;
		return;
	}
	session->sendInitiate(contentID, description, transport);
	onStateChange(FileTransfer::State(FileTransfer::State::WaitingForAccept));
}

void OutgoingJingleFileTransfer::handleRemoteTransportCandidateSelectFinished(JingleTransportPayload::ref payload) {
	if (canceled) {
		return;
	}
	if (JingleS5BTransportPayload::ref s5bPayload = boost::dynamic_pointer_cast<JingleS5BTransportPayload>(payload)) {
		ourCandidateChoice = s5bPayload;
		session->sendTransportInfo(contentID, s5bPayload);
		decideOnCandidates();
	}
}

void OutgoingJingleFileTransfer::replaceTransportWithIBB(const JingleContentID& id) {
	SWIFT_LOG(debug) << "Both parties failed. Replace transport with IBB." << std::endl;
	JingleIBBTransportPayload::ref ibbTransport = boost::make_shared<JingleIBBTransportPayload>();
	ibbTransport->setBlockSize(4096);
	ibbTransport->setSessionID(idGenerator->generateID());
	session->sendTransportReplace(id, ibbTransport);
}

void OutgoingJingleFileTransfer::handleTransferFinished(boost::optional<FileTransferError> error) {
	if (error) {
		session->sendTerminate(JinglePayload::Reason::ConnectivityError);
		onStateChange(FileTransfer::State(FileTransfer::State::Failed));
		onFinished(error);
	} else {
		sendSessionInfoHash();
		/*
		session->terminate(JinglePayload::Reason::Success);
		onStateChange(FileTransfer::State(FileTransfer::State::Finished));
		*/
	}
	//
}

}
