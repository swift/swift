/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/DefaultFileTransferTransporter.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/FileTransfer/FailingTransportSession.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>
#include <Swiften/FileTransfer/IBBReceiveTransportSession.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/IBBSendTransportSession.h>
#include <Swiften/FileTransfer/LocalJingleTransportCandidateGenerator.h>
#include <Swiften/FileTransfer/RemoteJingleTransportCandidateSelector.h>
#include <Swiften/FileTransfer/S5BTransportSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServer.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/StringCodecs/Hexify.h>

using namespace Swift;

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
            s5bProxy(s5bProxy),
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
    DefaultFileTransferTransporter::stopGeneratingLocalCandidates();
    remoteCandidateSelector->onCandidateSelectFinished.disconnect(
        boost::bind(&DefaultFileTransferTransporter::handleRemoteCandidateSelectFinished, this, _1, _2));
    delete remoteCandidateSelector;

    localCandidateGenerator->onLocalTransportCandidatesGenerated.disconnect(
        boost::bind(&DefaultFileTransferTransporter::handleLocalCandidatesGenerated, this, _1));
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
    s5bProxy->connectToProxies(getSOCKS5DstAddr());
    onLocalCandidatesGenerated(s5bSessionID, candidates, getSOCKS5DstAddr());
}

void DefaultFileTransferTransporter::handleRemoteCandidateSelectFinished(
        const boost::optional<JingleS5BTransportPayload::Candidate>& candidate,
        std::shared_ptr<SOCKS5BytestreamClientSession> session) {
    remoteS5BClientSession = session;
    onRemoteCandidateSelectFinished(s5bSessionID, candidate);
}


void DefaultFileTransferTransporter::addRemoteCandidates(
        const std::vector<JingleS5BTransportPayload::Candidate>& candidates, const std::string& dstAddr) {
    remoteCandidateSelector->setSOCKS5DstAddr(dstAddr.empty() ? getRemoteCandidateSOCKS5DstAddr() : dstAddr);
    remoteCandidateSelector->addCandidates(candidates);
}

void DefaultFileTransferTransporter::startTryingRemoteCandidates() {
    remoteCandidateSelector->startSelectingCandidate();
}

void DefaultFileTransferTransporter::stopTryingRemoteCandidates() {
    remoteCandidateSelector->stopSelectingCandidate();
}

void DefaultFileTransferTransporter::handleActivateProxySessionResult(const std::string& sessionID, ErrorPayload::ref error) {
    onProxyActivated(sessionID, error);
}

void DefaultFileTransferTransporter::startActivatingProxy(const JID& proxyServiceJID) {
    // activate proxy
    SWIFT_LOG(debug) << "Start activating proxy " << proxyServiceJID.toString() << " with sid = " << s5bSessionID << "." << std::endl;
    S5BProxyRequest::ref proxyRequest = std::make_shared<S5BProxyRequest>();
    proxyRequest->setSID(s5bSessionID);
    proxyRequest->setActivate(role == Initiator ? responder : initiator);

    std::shared_ptr<GenericRequest<S5BProxyRequest> > request = std::make_shared<GenericRequest<S5BProxyRequest> >(IQ::Set, proxyServiceJID, proxyRequest, router);
    request->onResponse.connect(boost::bind(&DefaultFileTransferTransporter::handleActivateProxySessionResult, this, s5bSessionID, _2));
    request->send();
}

void DefaultFileTransferTransporter::stopActivatingProxy() {
    // TODO
    assert(false);
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createIBBSendSession(
        const std::string& sessionID, unsigned int blockSize, std::shared_ptr<ReadBytestream> stream) {
    if (s5bServerManager->getServer()) {
        closeLocalSession();
    }
    closeRemoteSession();
    std::shared_ptr<IBBSendSession> ibbSession = std::make_shared<IBBSendSession>(
            sessionID, initiator, responder, stream, router);
    ibbSession->setBlockSize(blockSize);
    return std::make_shared<IBBSendTransportSession>(ibbSession);
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createIBBReceiveSession(
        const std::string& sessionID, unsigned long long size, std::shared_ptr<WriteBytestream> stream) {
    if (s5bServerManager->getServer()) {
        closeLocalSession();
    }
    closeRemoteSession();
    std::shared_ptr<IBBReceiveSession> ibbSession = std::make_shared<IBBReceiveSession>(
            sessionID, initiator, responder, size, stream, router);
    return std::make_shared<IBBReceiveTransportSession>(ibbSession);
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createRemoteCandidateSession(
        std::shared_ptr<ReadBytestream> stream, const JingleS5BTransportPayload::Candidate& /* candidate */) {
    closeLocalSession();
    return std::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(
        remoteS5BClientSession, stream);
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createRemoteCandidateSession(
        std::shared_ptr<WriteBytestream> stream, const JingleS5BTransportPayload::Candidate& /* candidate */) {
    closeLocalSession();
    return std::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(
        remoteS5BClientSession, stream);
}

std::shared_ptr<SOCKS5BytestreamServerSession> DefaultFileTransferTransporter::getServerSession() {
    s5bRegistry->setHasBytestream(getSOCKS5DstAddr(), false);
    std::vector<std::shared_ptr<SOCKS5BytestreamServerSession> > serverSessions =
        s5bServerManager->getServer()->getSessions(getSOCKS5DstAddr());
    while (serverSessions.size() > 1) {
        std::shared_ptr<SOCKS5BytestreamServerSession> session = serverSessions.back();
        serverSessions.pop_back();
        session->stop();
    }
    return !serverSessions.empty() ? serverSessions.front() : std::shared_ptr<SOCKS5BytestreamServerSession>();
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createLocalCandidateSession(
        std::shared_ptr<ReadBytestream> stream, const JingleS5BTransportPayload::Candidate& candidate) {
    closeRemoteSession();
    std::shared_ptr<TransportSession> transportSession;
    if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
        SOCKS5BytestreamClientSession::ref proxySession = s5bProxy->getProxySessionAndCloseOthers(candidate.jid, getLocalCandidateSOCKS5DstAddr());
        if (proxySession) {
            transportSession = std::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(proxySession, stream);
        }
        else {
            SWIFT_LOG(error) << "Failed obtaining proxy session with candidate JID " << candidate.jid << " and dstAddr " << getLocalCandidateSOCKS5DstAddr() << "." << std::endl;
        }
    }

    if (!transportSession) {
        std::shared_ptr<SOCKS5BytestreamServerSession> serverSession = getServerSession();
        if (serverSession) {
            transportSession = std::make_shared<S5BTransportSession<SOCKS5BytestreamServerSession> >(serverSession, stream);
        }
    }

    if (!transportSession) {
        transportSession = std::make_shared<FailingTransportSession>();
    }
    return transportSession;
}

std::shared_ptr<TransportSession> DefaultFileTransferTransporter::createLocalCandidateSession(
        std::shared_ptr<WriteBytestream> stream, const JingleS5BTransportPayload::Candidate& candidate) {
    closeRemoteSession();
    std::shared_ptr<TransportSession> transportSession;
    if (candidate.type == JingleS5BTransportPayload::Candidate::ProxyType) {
        SOCKS5BytestreamClientSession::ref proxySession = s5bProxy->getProxySessionAndCloseOthers(candidate.jid, getLocalCandidateSOCKS5DstAddr());
        if (proxySession) {
            transportSession = std::make_shared<S5BTransportSession<SOCKS5BytestreamClientSession> >(proxySession, stream);
        }
        else {
            SWIFT_LOG(error) << "Failed obtaining proxy session with candidate JID " << candidate.jid << " and dstAddr " << getLocalCandidateSOCKS5DstAddr() << "." << std::endl;
        }
    }

    if (!transportSession) {
        std::shared_ptr<SOCKS5BytestreamServerSession> serverSession = getServerSession();
        if (serverSession) {
            transportSession = std::make_shared<S5BTransportSession<SOCKS5BytestreamServerSession> >(serverSession, stream);
        }
    }

    if (!transportSession) {
        transportSession = std::make_shared<FailingTransportSession>();
    }
    return transportSession;
}

std::string DefaultFileTransferTransporter::getSOCKS5DstAddr() const {
    std::string result;
    if (role == Initiator) {
        result = getInitiatorCandidateSOCKS5DstAddr();
        SWIFT_LOG(debug) << "Initiator S5B DST.ADDR = " << s5bSessionID << " + " << initiator.toString() << " + " << responder.toString() << " : " << result << std::endl;
    }
    else {
        result = getResponderCandidateSOCKS5DstAddr();
        SWIFT_LOG(debug) << "Responder S5B DST.ADDR = " << s5bSessionID << " + " << responder.toString() << " + " << initiator.toString() << " : " << result << std::endl;
    }
    return result;
}

std::string DefaultFileTransferTransporter::getInitiatorCandidateSOCKS5DstAddr() const {
    return Hexify::hexify(crypto->getSHA1Hash(createSafeByteArray(s5bSessionID + initiator.toString() + responder.toString())));
}

std::string DefaultFileTransferTransporter::getResponderCandidateSOCKS5DstAddr() const {
    return Hexify::hexify(crypto->getSHA1Hash(createSafeByteArray(s5bSessionID + responder.toString() + initiator.toString())));
}

std::string DefaultFileTransferTransporter::getRemoteCandidateSOCKS5DstAddr() const {
    if (role == Initiator) {
        return getResponderCandidateSOCKS5DstAddr();
    }
    else {
        return getInitiatorCandidateSOCKS5DstAddr();
    }
}

std::string DefaultFileTransferTransporter::getLocalCandidateSOCKS5DstAddr() const {
    if (role == Responder) {
        return getResponderCandidateSOCKS5DstAddr();
    }
    else {
        return getInitiatorCandidateSOCKS5DstAddr();
    }
}

void DefaultFileTransferTransporter::closeLocalSession() {
    s5bRegistry->setHasBytestream(getSOCKS5DstAddr(), false);
    if (s5bServerManager->getServer()) {
        std::vector<std::shared_ptr<SOCKS5BytestreamServerSession> > serverSessions = s5bServerManager->getServer()->getSessions(getSOCKS5DstAddr());
        for (auto&& session : serverSessions) {
            session->stop();
        }
    }
}

void DefaultFileTransferTransporter::closeRemoteSession() {
    if (remoteS5BClientSession) {
        remoteS5BClientSession->stop();
        remoteS5BClientSession.reset();
    }
}
