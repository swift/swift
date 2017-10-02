/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/FileTransfer/FileTransferOptions.h>
#include <Swiften/FileTransfer/FileTransferTransporter.h>
#include <Swiften/FileTransfer/FileTransferTransporterFactory.h>
#include <Swiften/FileTransfer/IBBReceiveSession.h>
#include <Swiften/FileTransfer/IBBReceiveTransportSession.h>
#include <Swiften/FileTransfer/IBBSendSession.h>
#include <Swiften/FileTransfer/IBBSendTransportSession.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamProxiesManager.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>
#include <Swiften/FileTransfer/TransportSession.h>
#include <Swiften/JID/JID.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

class DummyFileTransferTransporter : public FileTransferTransporter {
public:
    enum Role {
        Initiator,
        Responder
    };

public:
    DummyFileTransferTransporter(
            const JID& initiator,
            const JID& responder,
            Role role,
            SOCKS5BytestreamRegistry* s5bRegistry,
            SOCKS5BytestreamServerManager* /* s5bServerManager */,
            SOCKS5BytestreamProxiesManager* /* s5bProxy */,
            IDGenerator* /* idGenerator */,
            ConnectionFactory*,
            TimerFactory*,
            CryptoProvider* cryptoProvider,
            IQRouter* iqRouter,
            const FileTransferOptions& ftOptions) : initiator_(initiator), responder_(responder), role_(role), s5bRegistry_(s5bRegistry), crypto_(cryptoProvider), iqRouter_(iqRouter), ftOptions_(ftOptions) {

    }

    void initialize() {
        s5bSessionID_ = s5bRegistry_->generateSessionID();
    }

    virtual void startGeneratingLocalCandidates() {
        std::vector<JingleS5BTransportPayload::Candidate> candidates;
        if (ftOptions_.isDirectAllowed()) {
            JingleS5BTransportPayload::Candidate candidate;
            candidate.cid = "123";
            candidate.priority = 1235;
            candidates.push_back(candidate);
        }
        onLocalCandidatesGenerated(s5bSessionID_, candidates, getSOCKS5DstAddr());
    }

    virtual void stopGeneratingLocalCandidates() {
    }

    virtual void addRemoteCandidates(const std::vector<JingleS5BTransportPayload::Candidate>&, const std::string&) {
    }

    virtual void startTryingRemoteCandidates() {
        onRemoteCandidateSelectFinished(s5bSessionID_, boost::optional<JingleS5BTransportPayload::Candidate>());
    }

    virtual void stopTryingRemoteCandidates() {
    }

    virtual void startActivatingProxy(const JID& /* proxy */) {
    }

    virtual void stopActivatingProxy() {
    }

    virtual std::shared_ptr<TransportSession> createIBBSendSession(const std::string& sessionID, unsigned int blockSize, std::shared_ptr<ReadBytestream> stream) {
        std::shared_ptr<IBBSendSession> ibbSession = std::make_shared<IBBSendSession>(
                sessionID, initiator_, responder_, stream, iqRouter_);
        ibbSession->setBlockSize(blockSize);
        return std::make_shared<IBBSendTransportSession>(ibbSession);
    }

    virtual std::shared_ptr<TransportSession> createIBBReceiveSession(const std::string& sessionID, unsigned long long size, std::shared_ptr<WriteBytestream> stream) {
        std::shared_ptr<IBBReceiveSession> ibbSession = std::make_shared<IBBReceiveSession>(
                sessionID, initiator_, responder_, size, stream, iqRouter_);
        return std::make_shared<IBBReceiveTransportSession>(ibbSession);
    }

    virtual std::shared_ptr<TransportSession> createRemoteCandidateSession(
            std::shared_ptr<ReadBytestream>, const JingleS5BTransportPayload::Candidate& /* candidate */) {
        return std::shared_ptr<TransportSession>();
    }

    virtual std::shared_ptr<TransportSession> createRemoteCandidateSession(
            std::shared_ptr<WriteBytestream>, const JingleS5BTransportPayload::Candidate& /* candidate */) {
        return std::shared_ptr<TransportSession>();
    }

    virtual std::shared_ptr<TransportSession> createLocalCandidateSession(
            std::shared_ptr<ReadBytestream>, const JingleS5BTransportPayload::Candidate& /* candidate */) {
        return std::shared_ptr<TransportSession>();
    }

    virtual std::shared_ptr<TransportSession> createLocalCandidateSession(
            std::shared_ptr<WriteBytestream>, const JingleS5BTransportPayload::Candidate& /* candidate */) {
        return std::shared_ptr<TransportSession>();
    }

private:
    std::string getSOCKS5DstAddr() const {
        std::string result;
        if (role_ == Initiator) {
            result = getInitiatorCandidateSOCKS5DstAddr();
        }
        else {
            result = getResponderCandidateSOCKS5DstAddr();
        }
        return result;
    }

    std::string getInitiatorCandidateSOCKS5DstAddr() const {
        return Hexify::hexify(crypto_->getSHA1Hash(createSafeByteArray(s5bSessionID_ + initiator_.toString() + responder_.toString())));
    }

    std::string getResponderCandidateSOCKS5DstAddr() const {
        return Hexify::hexify(crypto_->getSHA1Hash(createSafeByteArray(s5bSessionID_ + responder_.toString() + initiator_.toString())));
    }


private:
    JID initiator_;
    JID responder_;
    Role role_;
    SOCKS5BytestreamRegistry* s5bRegistry_;
    CryptoProvider* crypto_;
    std::string s5bSessionID_;
    IQRouter* iqRouter_;
    FileTransferOptions ftOptions_;
};

class DummyFileTransferTransporterFactory : public FileTransferTransporterFactory {
public:
        DummyFileTransferTransporterFactory(
            SOCKS5BytestreamRegistry* s5bRegistry,
            SOCKS5BytestreamServerManager* s5bServerManager,
            SOCKS5BytestreamProxiesManager* s5bProxy,
            IDGenerator* idGenerator,
            ConnectionFactory* connectionFactory,
            TimerFactory* timerFactory,
            CryptoProvider* cryptoProvider,
            IQRouter* iqRouter) : s5bRegistry_(s5bRegistry), s5bServerManager_(s5bServerManager), s5bProxy_(s5bProxy), idGenerator_(idGenerator), connectionFactory_(connectionFactory), timerFactory_(timerFactory), cryptoProvider_(cryptoProvider), iqRouter_(iqRouter) {

        }

    virtual ~DummyFileTransferTransporterFactory() {
    }

    virtual FileTransferTransporter* createInitiatorTransporter(const JID& initiator, const JID& responder, const FileTransferOptions& options) {
            DummyFileTransferTransporter* transporter = new DummyFileTransferTransporter(
                initiator,
                responder,
                DummyFileTransferTransporter::Initiator,
                s5bRegistry_,
                s5bServerManager_,
                s5bProxy_,
                idGenerator_,
                connectionFactory_,
                timerFactory_,
                cryptoProvider_,
                iqRouter_,
                options);
            transporter->initialize();
            return transporter;
    }

    virtual FileTransferTransporter* createResponderTransporter(const JID& /* initiator */, const JID& /* responder */, const std::string& /* s5bSessionID */, const FileTransferOptions& /* options */) {
        return nullptr;
    }

private:
    SOCKS5BytestreamRegistry* s5bRegistry_;
    SOCKS5BytestreamServerManager* s5bServerManager_;
    SOCKS5BytestreamProxiesManager* s5bProxy_;
    IDGenerator* idGenerator_;
    ConnectionFactory* connectionFactory_;
    TimerFactory* timerFactory_;
    CryptoProvider* cryptoProvider_;
    IQRouter* iqRouter_;
};

}
