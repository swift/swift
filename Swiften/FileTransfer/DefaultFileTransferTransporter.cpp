/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/DefaultFileTransferTransporter.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Queries/GenericRequest.h>

using namespace Swift;

namespace {
	class IBBSendTransportSession : public TransportSession {
		public:
			IBBSendTransportSession(boost::shared_ptr<IBBSendSession> session) : session(session) {
				finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
				bytesSentConnection = session->onBytesSent.connect(boost::bind(boost::ref(onBytesSent), _1));
			}

			virtual void start() SWIFTEN_OVERRIDE {
				session->start();
			}

			virtual void stop() SWIFTEN_OVERRIDE {
				session->stop();
			}

		private:
			boost::shared_ptr<IBBSendSession> session;
			boost::bsignals::scoped_connection finishedConnection;
			boost::bsignals::scoped_connection bytesSentConnection;
	};

	class IBBReceiveTransportSession : public TransportSession {
		public:
			IBBReceiveTransportSession(boost::shared_ptr<IBBReceiveSession> session) : session(session) {
				finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
			}

			virtual void start() SWIFTEN_OVERRIDE {
				session->start();
			}

			virtual void stop() SWIFTEN_OVERRIDE {
				session->stop();
			}

		private:
			boost::shared_ptr<IBBReceiveSession> session;
			boost::bsignals::scoped_connection finishedConnection;
			boost::bsignals::scoped_connection bytesSentConnection;
	};

	class FailingTransportSession : public TransportSession {
		public:
			virtual void start() SWIFTEN_OVERRIDE {
				onFinished(FileTransferError(FileTransferError::PeerError));
			}

			virtual void stop() SWIFTEN_OVERRIDE {
			}
	};

	template <typename T>
	class S5BTransportSession : public TransportSession {
		public:
			S5BTransportSession(
					boost::shared_ptr<T> session,
					boost::shared_ptr<ReadBytestream> readStream) : 
						session(session),
						readStream(readStream) {
				initialize();
			}

			S5BTransportSession(
					boost::shared_ptr<T> session,
					boost::shared_ptr<WriteBytestream> writeStream) : 
						session(session),
						writeStream(writeStream) {
				initialize();
			}

			virtual void start() SWIFTEN_OVERRIDE {
				if (readStream) {
					session->startSending(readStream);
				}
				else {
					session->startReceiving(writeStream);
				}
			}

			virtual void stop() SWIFTEN_OVERRIDE {
				session->stop();
			}

		private:
			void initialize() {
				finishedConnection = session->onFinished.connect(boost::bind(boost::ref(onFinished), _1));
				bytesSentConnection = session->onBytesSent.connect(boost::bind(boost::ref(onBytesSent), _1));
			}

		private:
			boost::shared_ptr<T> session;
			boost::shared_ptr<ReadBytestream> readStream;
			boost::shared_ptr<WriteBytestream> writeStream;

			boost::bsignals::scoped_connection finishedConnection;
			boost::bsignals::scoped_connection bytesSentConnection;
	};
}

DefaultFileTransferTransporter::DefaultFileTransferTransporter(
		const JID& initiator, 
		const JID& responder,
		Role role,
		SOCKS5BytestreamRegistry* s5bRegistry,
		SOCKS5BytestreamServerManager* s5bServerManager,
		SOCKS5BytestreamProxiesManager* s5bProxy, 
		IDGenerator* idGenerator, 
		ConnectionFactory* connectionFactory, 
		TimerFactory* timerFactory, 
		CryptoProvider* crypto,
		IQRouter* router,
		const FileTransferOptions& options) :
			initiator(initiator),
			responder(responder),
			role(role),
			s5bRegistry(s5bRegistry),
			s5bServerManager(s5bServerManager),
			crypto(crypto),
			router(router) {

	localCandidateGenerator = new LocalJingleTransportCandidateGenerator(
			s5bServerManager,
			s5bProxy,
			role == Initiator ? initiator : responder,
			idGenerator,
			options);
	localCandidateGenerator->onLocalTransportCandidatesGenerated.connect(
		boost::bind(&DefaultFileTransferTransporter::handleLocalCandidatesGenerated, this, _1));

	remoteCandidateSelector = new RemoteJingleTransportCandidateSelector(
			connectionFactory,
			timerFactory,
			options);
	remoteCandidateSelector->onCandidateSelectFinished.connect(
		boost::bind(&DefaultFileTransferTransporter::handleRemoteCandidateSelectFinished, this, _1, _2));
}

DefaultFileTransferTransporter::~DefaultFileTransferTransporter() {
	delete remoteCandidateSelector;
	delete localCandidateGenerator;
}

void DefaultFileTransferTransporter::initialize() {
	s5bSessionID = s5bRegistry->generateSessionID();
}

void DefaultFileTransferTransporter::initialize(const std::string& s5bSessionID) {
	this->s5bSessionID = s5bSessionID;
}

void DefaultFileTransferTransporter::startGeneratingLocalCandidates() {
	localCandidateGenerator->start();
}

void DefaultFileTransferTransporter::stopGeneratingLocalCandidates() {
	localCandidateGenerator->stop();
}
			
void DefaultFileTransferTransporter::handleLocalCandidatesGenerated(
		const std::vector<JingleS5BTransportPayload::Candidate>& candidates) {
	s5bRegistry->setHasBytestream(getSOCKS5DstAddr(), true);
	onLocalCandidatesGenerated(s5bSessionID, candidates);
}

void DefaultFileTransferTransporter::handleRemoteCandidateSelectFinished(
		const boost::optional<JingleS5BTransportPayload::Candidate>& candidate, 
		boost::shared_ptr<SOCKS5BytestreamClientSession> session) {
	remoteS5BClientSession = session;
	onRemoteCandidateSelectFinished(s5bSessionID, candidate);
}


void DefaultFileTransferTransporter::addRemoteCandidates(
		const std::vector<JingleS5BTransportPayload::Candidate>& candidates) {
	remoteCandidateSelector->setSOCKS5DstAddr(getSOCKS5DstAddr());
	remoteCandidateSelector->addCandidates(candidates);
}

void DefaultFileTransferTransporter::startTryingRemoteCandidates() {
	remoteCandidateSelector->startSelectingCandidate();
}

void DefaultFileTransferTransporter::stopTryingRemoteCandidates() {
	remoteCandidateSelector->stopSelectingCandidate();
}

void DefaultFileTransferTransporter::startActivatingProxy(const JID&) {
	// TODO
	assert(false);
	/*
	S5BProxyRequest::ref proxyRequest = boost::make_shared<S5BProxyRequest>();
	proxyRequest->setSID(s5bSessionID);
	proxyRequest->setActivate(getTarget());

	boost::shared_ptr<GenericRequest<S5BProxyRequest> > request = boost::make_shared<GenericRequest<S5BProxyRequest> >(IQ::Set, proxy, proxyRequest, router);
	request->onResponse.connect(boost::bind(&OutgoingJingleFileTransfer::handleActivateProxySessionResult, this, _1, _2));
	request->send();
	*/
}

void DefaultFileTransferTransporter::stopActivatingProxy() {
	// TODO
	assert(false);
}

boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createIBBSendSession(
		const std::string& sessionID, unsigned int blockSize, boost::shared_ptr<ReadBytestream> stream) {
	closeLocalSession();
	closeRemoteSession();
	boost::shared_ptr<IBBSendSession> ibbSession = boost::make_shared<IBBSendSession>(
			sessionID, initiator, responder, stream, router);
	ibbSession->setBlockSize(blockSize);
	return boost::make_shared<IBBSendTransportSession>(ibbSession);
}

boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createIBBReceiveSession(
		const std::string& sessionID, unsigned long long size, boost::shared_ptr<WriteBytestream> stream) {
	if (s5bServerManager->getServer()) {
		closeLocalSession();
		closeRemoteSession();
	}
	boost::shared_ptr<IBBReceiveSession> ibbSession = boost::make_shared<IBBReceiveSession>(
			sessionID, initiator, responder, size, stream, router);
	return boost::make_shared<IBBReceiveTransportSession>(ibbSession);
}

boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createRemoteCandidateSession(
		boost::shared_ptr<ReadBytestream> stream) {
	closeLocalSession();
	return boost::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(
		remoteS5BClientSession, stream);
}

boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createRemoteCandidateSession(
		boost::shared_ptr<WriteBytestream> stream) {
	closeLocalSession();
	return boost::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(
		remoteS5BClientSession, stream);
}

boost::shared_ptr<SOCKS5BytestreamServerSession> DefaultFileTransferTransporter::getServerSession() {
	s5bRegistry->setHasBytestream(getSOCKS5DstAddr(), false);
	std::vector<boost::shared_ptr<SOCKS5BytestreamServerSession> > serverSessions = 
		s5bServerManager->getServer()->getSessions(getSOCKS5DstAddr());
	while (serverSessions.size() > 1) {
		boost::shared_ptr<SOCKS5BytestreamServerSession> session = serverSessions.back();
		serverSessions.pop_back();
		session->stop();
	}
	return !serverSessions.empty() ? serverSessions.front() : boost::shared_ptr<SOCKS5BytestreamServerSession>();
}


boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createLocalCandidateSession(
		boost::shared_ptr<ReadBytestream> stream) {
	closeRemoteSession();
	boost::shared_ptr<SOCKS5BytestreamServerSession> serverSession = getServerSession();
	if (serverSession) {
		return boost::make_shared<S5BTransportSession<SOCKS5BytestreamServerSession> >(serverSession, stream);
	}
	else {
		return boost::make_shared<FailingTransportSession>();
	}
}

boost::shared_ptr<TransportSession> DefaultFileTransferTransporter::createLocalCandidateSession(
		boost::shared_ptr<WriteBytestream> stream) {
	closeRemoteSession();
	boost::shared_ptr<SOCKS5BytestreamServerSession> serverSession = getServerSession();
	if (serverSession) {
		return boost::make_shared<S5BTransportSession<SOCKS5BytestreamServerSession> >(serverSession, stream);
	}
	else {
		return boost::make_shared<FailingTransportSession>();
	}
}

std::string DefaultFileTransferTransporter::getSOCKS5DstAddr() const {
	return Hexify::hexify(crypto->getSHA1Hash(
				createSafeByteArray(s5bSessionID + initiator.toString() + initiator.toString())));
}

void DefaultFileTransferTransporter::closeLocalSession() {
	s5bRegistry->setHasBytestream(getSOCKS5DstAddr(), false);
	std::vector<boost::shared_ptr<SOCKS5BytestreamServerSession> > serverSessions = 
		s5bServerManager->getServer()->getSessions(getSOCKS5DstAddr());
	foreach(boost::shared_ptr<SOCKS5BytestreamServerSession> session, serverSessions) {
		session->stop();
	}
}

void DefaultFileTransferTransporter::closeRemoteSession() {
	if (remoteS5BClientSession) {
		remoteS5BClientSession->stop();
		remoteS5BClientSession.reset();
	}
}
