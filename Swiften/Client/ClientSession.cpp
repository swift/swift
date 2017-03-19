/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/ClientSession.h>

#include <memory>

#include <boost/bind.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Platform.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Elements/AuthChallenge.h>
#include <Swiften/Elements/AuthFailure.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/Elements/AuthSuccess.h>
#include <Swiften/Elements/CompressFailure.h>
#include <Swiften/Elements/CompressRequest.h>
#include <Swiften/Elements/Compressed.h>
#include <Swiften/Elements/EnableStreamManagement.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/ProtocolHeader.h>
#include <Swiften/Elements/ResourceBind.h>
#include <Swiften/Elements/StanzaAck.h>
#include <Swiften/Elements/StanzaAckRequest.h>
#include <Swiften/Elements/StartSession.h>
#include <Swiften/Elements/StartTLSFailure.h>
#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Elements/StreamError.h>
#include <Swiften/Elements/StreamFeatures.h>
#include <Swiften/Elements/StreamManagementEnabled.h>
#include <Swiften/Elements/StreamManagementFailed.h>
#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/SASL/DIGESTMD5ClientAuthenticator.h>
#include <Swiften/SASL/EXTERNALClientAuthenticator.h>
#include <Swiften/SASL/PLAINClientAuthenticator.h>
#include <Swiften/SASL/SCRAMSHA1ClientAuthenticator.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/Session/BasicSessionStream.h>
#include <Swiften/Session/BOSHSessionStream.h>
#include <Swiften/StreamManagement/StanzaAckRequester.h>
#include <Swiften/StreamManagement/StanzaAckResponder.h>
#include <Swiften/TLS/CertificateTrustChecker.h>
#include <Swiften/TLS/ServerIdentityVerifier.h>

#ifdef SWIFTEN_PLATFORM_WIN32
#include <Swiften/Base/WindowsRegistry.h>
#include <Swiften/SASL/WindowsGSSAPIClientAuthenticator.h>
#endif

#define CHECK_STATE_OR_RETURN(a) \
    if (!checkState(a)) { return; }

namespace Swift {

ClientSession::ClientSession(
        const JID& jid,
        std::shared_ptr<SessionStream> stream,
        IDNConverter* idnConverter,
        CryptoProvider* crypto,
        TimerFactory* timerFactory) :
            localJID(jid),
            state(State::Initial),
            stream(stream),
            idnConverter(idnConverter),
            crypto(crypto),
            timerFactory(timerFactory),
            allowPLAINOverNonTLS(false),
            useStreamCompression(true),
            useTLS(UseTLSWhenAvailable),
            useAcks(true),
            needSessionStart(false),
            needResourceBind(false),
            needAcking(false),
            rosterVersioningSupported(false),
            authenticator(nullptr),
            certificateTrustChecker(nullptr),
            singleSignOn(false),
            authenticationPort(-1) {
#ifdef SWIFTEN_PLATFORM_WIN32
if (WindowsRegistry::isFIPSEnabled()) {
    SWIFT_LOG(info) << "Windows is running in FIPS-140 mode. Some authentication methods will be unavailable." << std::endl;
}
#endif
}

ClientSession::~ClientSession() {
}

void ClientSession::start() {
    stream->onStreamStartReceived.connect(boost::bind(&ClientSession::handleStreamStart, shared_from_this(), _1));
    stream->onStreamEndReceived.connect(boost::bind(&ClientSession::handleStreamEnd, shared_from_this()));
    stream->onElementReceived.connect(boost::bind(&ClientSession::handleElement, shared_from_this(), _1));
    stream->onClosed.connect(boost::bind(&ClientSession::handleStreamClosed, shared_from_this(), _1));
    stream->onTLSEncrypted.connect(boost::bind(&ClientSession::handleTLSEncrypted, shared_from_this()));

    assert(state == State::Initial);
    state = State::WaitingForStreamStart;
    sendStreamHeader();
}

void ClientSession::sendStreamHeader() {
    ProtocolHeader header;
    header.setTo(getRemoteJID());
    stream->writeHeader(header);
}

void ClientSession::sendStanza(std::shared_ptr<Stanza> stanza) {
    stream->writeElement(stanza);
    if (stanzaAckRequester_) {
        stanzaAckRequester_->handleStanzaSent(stanza);
    }
}

void ClientSession::handleStreamStart(const ProtocolHeader&) {
    CHECK_STATE_OR_RETURN(State::WaitingForStreamStart);
    state = State::Negotiating;
}

void ClientSession::handleStreamEnd() {
    if (state == State::Finishing) {
        // We are already in finishing state if we iniated the close of the session.
        stream->close();
    }
    else {
        state = State::Finishing;
        initiateShutdown(true);
    }
}

void ClientSession::handleElement(std::shared_ptr<ToplevelElement> element) {
    if (std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element)) {
        if (stanzaAckResponder_) {
            stanzaAckResponder_->handleStanzaReceived();
        }
        if (getState() == State::Initialized) {
            onStanzaReceived(stanza);
        }
        else if (std::shared_ptr<IQ> iq = std::dynamic_pointer_cast<IQ>(element)) {
            if (state == State::BindingResource) {
                std::shared_ptr<ResourceBind> resourceBind(iq->getPayload<ResourceBind>());
                if (iq->getType() == IQ::Error && iq->getID() == "session-bind") {
                    finishSession(Error::ResourceBindError);
                }
                else if (!resourceBind) {
                    finishSession(Error::UnexpectedElementError);
                }
                else if (iq->getType() == IQ::Result) {
                    localJID = resourceBind->getJID();
                    if (!localJID.isValid()) {
                        finishSession(Error::ResourceBindError);
                    }
                    needResourceBind = false;
                    continueSessionInitialization();
                }
                else {
                    finishSession(Error::UnexpectedElementError);
                }
            }
            else if (state == State::StartingSession) {
                if (iq->getType() == IQ::Result) {
                    needSessionStart = false;
                    continueSessionInitialization();
                }
                else if (iq->getType() == IQ::Error) {
                    finishSession(Error::SessionStartError);
                }
                else {
                    finishSession(Error::UnexpectedElementError);
                }
            }
            else {
                finishSession(Error::UnexpectedElementError);
            }
        }
    }
    else if (std::dynamic_pointer_cast<StanzaAckRequest>(element)) {
        if (stanzaAckResponder_) {
            stanzaAckResponder_->handleAckRequestReceived();
        }
    }
    else if (std::shared_ptr<StanzaAck> ack = std::dynamic_pointer_cast<StanzaAck>(element)) {
        if (stanzaAckRequester_) {
            if (ack->isValid()) {
                stanzaAckRequester_->handleAckReceived(ack->getHandledStanzasCount());
            }
            else {
                SWIFT_LOG(warning) << "Got invalid ack from server";
            }
        }
        else {
            SWIFT_LOG(warning) << "Ignoring ack";
        }
    }
    else if (StreamError::ref streamError = std::dynamic_pointer_cast<StreamError>(element)) {
        finishSession(Error::StreamError);
    }
    else if (getState() == State::Initialized) {
        std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element);
        if (stanza) {
            if (stanzaAckResponder_) {
                stanzaAckResponder_->handleStanzaReceived();
            }
            onStanzaReceived(stanza);
        }
    }
    else if (StreamFeatures* streamFeatures = dynamic_cast<StreamFeatures*>(element.get())) {
        CHECK_STATE_OR_RETURN(State::Negotiating);

        if (streamFeatures->hasStartTLS() && stream->supportsTLSEncryption() && useTLS != NeverUseTLS) {
            state = State::WaitingForEncrypt;
            stream->writeElement(std::make_shared<StartTLSRequest>());
        }
        else if (useTLS == RequireTLS && !stream->isTLSEncrypted()) {
            finishSession(Error::NoSupportedAuthMechanismsError);
        }
        else if (useStreamCompression && stream->supportsZLibCompression() && streamFeatures->hasCompressionMethod("zlib")) {
            state = State::Compressing;
            stream->writeElement(std::make_shared<CompressRequest>("zlib"));
        }
        else if (streamFeatures->hasAuthenticationMechanisms()) {
#ifdef SWIFTEN_PLATFORM_WIN32
            if (singleSignOn) {
                const boost::optional<std::string> authenticationHostname = streamFeatures->getAuthenticationHostname();
                bool gssapiSupported = streamFeatures->hasAuthenticationMechanism("GSSAPI") && authenticationHostname && !authenticationHostname->empty();

                if (!gssapiSupported) {
                    finishSession(Error::NoSupportedAuthMechanismsError);
                }
                else {
                    WindowsGSSAPIClientAuthenticator* gssapiAuthenticator = new WindowsGSSAPIClientAuthenticator(*authenticationHostname, localJID.getDomain(), authenticationPort);
                    std::shared_ptr<Error> error = std::make_shared<Error>(Error::AuthenticationFailedError);

                    authenticator = gssapiAuthenticator;

                    if (!gssapiAuthenticator->isError()) {
                        state = State::Authenticating;
                        stream->writeElement(std::make_shared<AuthRequest>(authenticator->getName(), authenticator->getResponse()));
                    }
                    else {
                        error->errorCode = gssapiAuthenticator->getErrorCode();
                        finishSession(error);
                    }
                }
            }
            else
#endif
            if (stream->hasTLSCertificate()) {
                if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
                    authenticator = new EXTERNALClientAuthenticator();
                    state = State::Authenticating;
                    stream->writeElement(std::make_shared<AuthRequest>("EXTERNAL", createSafeByteArray("")));
                }
                else {
                    finishSession(Error::TLSClientCertificateError);
                }
            }
            else if (streamFeatures->hasAuthenticationMechanism("EXTERNAL")) {
                authenticator = new EXTERNALClientAuthenticator();
                state = State::Authenticating;
                stream->writeElement(std::make_shared<AuthRequest>("EXTERNAL", createSafeByteArray("")));
            }
            else if (streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1") || streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1-PLUS")) {
                std::ostringstream s;
                ByteArray finishMessage;
                bool plus = streamFeatures->hasAuthenticationMechanism("SCRAM-SHA-1-PLUS");
                if (stream->isTLSEncrypted()) {
                    finishMessage = stream->getTLSFinishMessage();
                    plus &= !finishMessage.empty();
                }
                s << boost::uuids::random_generator()();
                SCRAMSHA1ClientAuthenticator* scramAuthenticator = new SCRAMSHA1ClientAuthenticator(s.str(), plus, idnConverter, crypto);
                if (!finishMessage.empty()) {
                    scramAuthenticator->setTLSChannelBindingData(finishMessage);
                }
                authenticator = scramAuthenticator;
                state = State::WaitingForCredentials;
                onNeedCredentials();
            }
            else if ((stream->isTLSEncrypted() || allowPLAINOverNonTLS) && streamFeatures->hasAuthenticationMechanism("PLAIN")) {
                authenticator = new PLAINClientAuthenticator();
                state = State::WaitingForCredentials;
                onNeedCredentials();
            }
            else if (streamFeatures->hasAuthenticationMechanism("DIGEST-MD5") && crypto->isMD5AllowedForCrypto()) {
                std::ostringstream s;
                s << boost::uuids::random_generator()();
                // FIXME: Host should probably be the actual host
                authenticator = new DIGESTMD5ClientAuthenticator(localJID.getDomain(), s.str(), crypto);
                state = State::WaitingForCredentials;
                onNeedCredentials();
            }
            else {
                finishSession(Error::NoSupportedAuthMechanismsError);
            }
        }
        else {
            // Start the session
            rosterVersioningSupported = streamFeatures->hasRosterVersioning();
            stream->setWhitespacePingEnabled(true);
            needSessionStart = streamFeatures->hasSession();
            needResourceBind = streamFeatures->hasResourceBind();
            needAcking = streamFeatures->hasStreamManagement() && useAcks;
            if (!needResourceBind) {
                // Resource binding is a MUST
                finishSession(Error::ResourceBindError);
            }
            else {
                continueSessionInitialization();
            }
        }
    }
    else if (std::dynamic_pointer_cast<Compressed>(element)) {
        CHECK_STATE_OR_RETURN(State::Compressing);
        state = State::WaitingForStreamStart;
        stream->addZLibCompression();
        stream->resetXMPPParser();
        sendStreamHeader();
    }
    else if (std::dynamic_pointer_cast<CompressFailure>(element)) {
        finishSession(Error::CompressionFailedError);
    }
    else if (std::dynamic_pointer_cast<StreamManagementEnabled>(element)) {
        stanzaAckRequester_ = std::make_shared<StanzaAckRequester>();
        stanzaAckRequester_->onRequestAck.connect(boost::bind(&ClientSession::requestAck, shared_from_this()));
        stanzaAckRequester_->onStanzaAcked.connect(boost::bind(&ClientSession::handleStanzaAcked, shared_from_this(), _1));
        stanzaAckResponder_ = std::make_shared<StanzaAckResponder>();
        stanzaAckResponder_->onAck.connect(boost::bind(&ClientSession::ack, shared_from_this(), _1));
        needAcking = false;
        continueSessionInitialization();
    }
    else if (std::dynamic_pointer_cast<StreamManagementFailed>(element)) {
        needAcking = false;
        continueSessionInitialization();
    }
    else if (AuthChallenge* challenge = dynamic_cast<AuthChallenge*>(element.get())) {
        CHECK_STATE_OR_RETURN(State::Authenticating);
        assert(authenticator);
        if (authenticator->setChallenge(challenge->getValue())) {
            stream->writeElement(std::make_shared<AuthResponse>(authenticator->getResponse()));
        }
#ifdef SWIFTEN_PLATFORM_WIN32
        else if (WindowsGSSAPIClientAuthenticator* gssapiAuthenticator = dynamic_cast<WindowsGSSAPIClientAuthenticator*>(authenticator)) {
            std::shared_ptr<Error> error = std::make_shared<Error>(Error::AuthenticationFailedError);

            error->errorCode = gssapiAuthenticator->getErrorCode();
            finishSession(error);
        }
#endif
        else {
            finishSession(Error::AuthenticationFailedError);
        }
    }
    else if (AuthSuccess* authSuccess = dynamic_cast<AuthSuccess*>(element.get())) {
        CHECK_STATE_OR_RETURN(State::Authenticating);
        assert(authenticator);
        if (!authenticator->setChallenge(authSuccess->getValue())) {
            finishSession(Error::ServerVerificationFailedError);
        }
        else {
            state = State::WaitingForStreamStart;
            delete authenticator;
            authenticator = nullptr;
            stream->resetXMPPParser();
            sendStreamHeader();
        }
    }
    else if (dynamic_cast<AuthFailure*>(element.get())) {
        finishSession(Error::AuthenticationFailedError);
    }
    else if (dynamic_cast<TLSProceed*>(element.get())) {
        CHECK_STATE_OR_RETURN(State::WaitingForEncrypt);
        state = State::Encrypting;
        stream->addTLSEncryption();
    }
    else if (dynamic_cast<StartTLSFailure*>(element.get())) {
        finishSession(Error::TLSError);
    }
    else {
        // FIXME Not correct?
        state = State::Initialized;
        onInitialized();
    }
}

void ClientSession::continueSessionInitialization() {
    if (needResourceBind) {
        state = State::BindingResource;
        std::shared_ptr<ResourceBind> resourceBind(std::make_shared<ResourceBind>());
        if (!localJID.getResource().empty()) {
            resourceBind->setResource(localJID.getResource());
        }
        sendStanza(IQ::createRequest(IQ::Set, JID(), "session-bind", resourceBind));
    }
    else if (needAcking) {
        state = State::EnablingSessionManagement;
        stream->writeElement(std::make_shared<EnableStreamManagement>());
    }
    else if (needSessionStart) {
        state = State::StartingSession;
        sendStanza(IQ::createRequest(IQ::Set, JID(), "session-start", std::make_shared<StartSession>()));
    }
    else {
        state = State::Initialized;
        onInitialized();
    }
}

bool ClientSession::checkState(State state) {
    if (this->state != state) {
        finishSession(Error::UnexpectedElementError);
        return false;
    }
    return true;
}

void ClientSession::sendCredentials(const SafeByteArray& password) {
    assert(state == State::WaitingForCredentials);
    assert(authenticator);
    state = State::Authenticating;
    authenticator->setCredentials(localJID.getNode(), password);
    stream->writeElement(std::make_shared<AuthRequest>(authenticator->getName(), authenticator->getResponse()));
}

void ClientSession::handleTLSEncrypted() {
    if (!std::dynamic_pointer_cast<BOSHSessionStream>(stream)) {
        CHECK_STATE_OR_RETURN(State::Encrypting);
    }

    std::vector<Certificate::ref> certificateChain = stream->getPeerCertificateChain();
    std::shared_ptr<CertificateVerificationError> verificationError = stream->getPeerCertificateVerificationError();
    if (verificationError) {
        checkTrustOrFinish(certificateChain, verificationError);
    }
    else {
        ServerIdentityVerifier identityVerifier(localJID, idnConverter);
        if (!certificateChain.empty() && identityVerifier.certificateVerifies(certificateChain[0])) {
            continueAfterTLSEncrypted();
        }
        else {
            checkTrustOrFinish(certificateChain, std::make_shared<CertificateVerificationError>(CertificateVerificationError::InvalidServerIdentity));
        }
    }
}

void ClientSession::checkTrustOrFinish(const std::vector<Certificate::ref>& certificateChain, std::shared_ptr<CertificateVerificationError> error) {
    if (certificateTrustChecker && certificateTrustChecker->isCertificateTrusted(certificateChain)) {
        if (!std::dynamic_pointer_cast<BOSHSessionStream>(stream)) {
            continueAfterTLSEncrypted();
        }
    }
    else {
        finishSession(error);
    }
}

void ClientSession::initiateShutdown(bool sendFooter) {
    if (!streamShutdownTimeout) {
        streamShutdownTimeout = timerFactory->createTimer(sessionShutdownTimeoutInMilliseconds);
        streamShutdownTimeout->onTick.connect(boost::bind(&ClientSession::handleStreamShutdownTimeout, shared_from_this()));
        streamShutdownTimeout->start();
    }
    if (sendFooter) {
        stream->writeFooter();
    }
    if (state == State::Finishing) {
        // The other side already send </stream>; we can close the socket.
        stream->close();
    }
    else {
        state = State::Finishing;
    }
}

void ClientSession::continueAfterTLSEncrypted() {
    if (!std::dynamic_pointer_cast<BOSHSessionStream>(stream)) {
        state = State::WaitingForStreamStart;
        stream->resetXMPPParser();
        sendStreamHeader();
    }
}

void ClientSession::handleStreamClosed(std::shared_ptr<Swift::Error> streamError) {
    State previousState = state;
    state = State::Finished;

    if (streamShutdownTimeout) {
        streamShutdownTimeout->stop();
        streamShutdownTimeout.reset();
    }

    if (stanzaAckRequester_) {
        stanzaAckRequester_->onRequestAck.disconnect(boost::bind(&ClientSession::requestAck, shared_from_this()));
        stanzaAckRequester_->onStanzaAcked.disconnect(boost::bind(&ClientSession::handleStanzaAcked, shared_from_this(), _1));
        stanzaAckRequester_.reset();
    }
    if (stanzaAckResponder_) {
        stanzaAckResponder_->onAck.disconnect(boost::bind(&ClientSession::ack, shared_from_this(), _1));
        stanzaAckResponder_.reset();
    }
    stream->setWhitespacePingEnabled(false);
    stream->onStreamStartReceived.disconnect(boost::bind(&ClientSession::handleStreamStart, shared_from_this(), _1));
    stream->onStreamEndReceived.disconnect(boost::bind(&ClientSession::handleStreamEnd, shared_from_this()));
    stream->onElementReceived.disconnect(boost::bind(&ClientSession::handleElement, shared_from_this(), _1));
    stream->onClosed.disconnect(boost::bind(&ClientSession::handleStreamClosed, shared_from_this(), _1));
    stream->onTLSEncrypted.disconnect(boost::bind(&ClientSession::handleTLSEncrypted, shared_from_this()));

    if (previousState == State::Finishing) {
        onFinished(error_);
    }
    else {
        onFinished(streamError);
    }
}

void ClientSession::handleStreamShutdownTimeout() {
    handleStreamClosed(std::shared_ptr<Swift::Error>());
}

void ClientSession::finish() {
    if (state != State::Finishing && state != State::Finished) {
        finishSession(std::shared_ptr<Error>());
    }
    else {
        SWIFT_LOG(warning) << "Session already finished or finishing." << std::endl;
    }
}

void ClientSession::finishSession(Error::Type error) {
    finishSession(std::make_shared<Swift::ClientSession::Error>(error));
}

void ClientSession::finishSession(std::shared_ptr<Swift::Error> error) {
    if (!error_) {
        error_ = error;
    }
    else {
        SWIFT_LOG(warning) << "Session finished twice" << std::endl;
    }
    assert(stream->isOpen());
    if (stanzaAckResponder_) {
        stanzaAckResponder_->handleAckRequestReceived();
    }
    if (authenticator) {
        delete authenticator;
        authenticator = nullptr;
    }
    // Immidiately close TCP connection without stream closure.
    if (std::dynamic_pointer_cast<CertificateVerificationError>(error)) {
        state = State::Finishing;
        initiateShutdown(false);
    }
    else {
        if (state == State::Finishing) {
            initiateShutdown(true);
        }
        else if (state != State::Finished) {
            initiateShutdown(true);
        }
    }
}

void ClientSession::requestAck() {
    stream->writeElement(std::make_shared<StanzaAckRequest>());
}

void ClientSession::handleStanzaAcked(std::shared_ptr<Stanza> stanza) {
    onStanzaAcked(stanza);
}

void ClientSession::ack(unsigned int handledStanzasCount) {
    stream->writeElement(std::make_shared<StanzaAck>(handledStanzasCount));
}

}
