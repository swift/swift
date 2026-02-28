/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Session/SessionStream.h>

namespace Swift {
    class CertificateTrustChecker;
    class ClientAuthenticator;
    class CryptoProvider;
    class IDNConverter;
    class Stanza;
    class StanzaAckRequester;
    class StanzaAckResponder;
    class TimerFactory;
    class Timer;

    class SWIFTEN_API ClientSession : public std::enable_shared_from_this<ClientSession> {
        public:
            enum class State {
                Initial,
                WaitingForStreamStart,
                Negotiating,
                Compressing,
                WaitingForEncrypt,
                Encrypting,
                WaitingForCredentials,
                Authenticating,
                EnablingSessionManagement,
                BindingResource,
                StartingSession,
                Initialized,
                Finishing,
                Finished
            };

            struct Error : public Swift::Error {
                enum Type {
                    AuthenticationFailedError,
                    CompressionFailedError,
                    ServerVerificationFailedError,
                    NoSupportedAuthMechanismsError,
                    UnexpectedElementError,
                    ResourceBindError,
                    SessionStartError,
                    TLSClientCertificateError,
                    TLSError,
                    StreamError,
                    StreamEndError, // The server send a closing stream tag.
                } type;
                std::shared_ptr<boost::system::error_code> errorCode;
                Error(Type type) : type(type) {}
            };

            enum UseTLS {
                NeverUseTLS,
                UseTLSWhenAvailable,
                RequireTLS
            };

            ~ClientSession();

            static std::shared_ptr<ClientSession> create(const JID& jid, std::shared_ptr<SessionStream> stream, IDNConverter* idnConverter, CryptoProvider* crypto, TimerFactory* timerFactory) {
                return std::shared_ptr<ClientSession>(new ClientSession(jid, stream, idnConverter, crypto, timerFactory));
            }

            State getState() const {
                return state;
            }

            void setAllowPLAINOverNonTLS(bool b) {
                allowPLAINOverNonTLS = b;
            }

            void setUseStreamCompression(bool b) {
                useStreamCompression = b;
            }

            void setUseTLS(UseTLS b) {
                useTLS = b;
            }

            void setUseAcks(bool b) {
                useAcks = b;
            }

            bool getStreamManagementEnabled() const {
                // Explicitly convert to bool. In C++11, it would be cleaner to
                // compare to nullptr.
                return static_cast<bool>(stanzaAckRequester_);
            }

            bool getRosterVersioningSupported() const {
                return rosterVersioningSupported;
            }

            std::vector<Certificate::ref> getPeerCertificateChain() const {
                return stream->getPeerCertificateChain();
            }

            const JID& getLocalJID() const {
                return localJID;
            }

            void start();
            void finish();

            bool isFinished() const {
                return getState() == State::Finished;
            }

            void sendCredentials(const SafeByteArray& password);
            void sendStanza(std::shared_ptr<Stanza>);

            void setCertificateTrustChecker(CertificateTrustChecker* checker) {
                certificateTrustChecker = checker;
            }

            void setSingleSignOn(bool b) {
                singleSignOn = b;
            }

            /**
             * Sets the port number used in Kerberos authentication
             * Does not affect network connectivity.
             */
            void setAuthenticationPort(int i) {
                authenticationPort = i;
            }

            void setSessionShutdownTimeout(int timeoutInMilliseconds) {
                sessionShutdownTimeoutInMilliseconds = timeoutInMilliseconds;
            }

        public:
            boost::signals2::signal<void ()> onNeedCredentials;
            boost::signals2::signal<void ()> onInitialized;
            boost::signals2::signal<void (std::shared_ptr<Swift::Error>)> onFinished;
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaReceived;
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaAcked;

        private:
            ClientSession(
                    const JID& jid,
                    std::shared_ptr<SessionStream>,
                    IDNConverter* idnConverter,
                    CryptoProvider* crypto,
                    TimerFactory* timerFactory);

            void finishSession(Error::Type error);
            void finishSession(std::shared_ptr<Swift::Error> error);

            JID getRemoteJID() const {
                return JID("", localJID.getDomain());
            }

            void sendStreamHeader();

            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamStart(const ProtocolHeader&);
            void handleStreamEnd();
            void handleStreamClosed(std::shared_ptr<Swift::Error>);
            void handleStreamShutdownTimeout();

            void handleTLSEncrypted();

            bool checkState(State);
            void continueSessionInitialization();

            void requestAck();
            void handleStanzaAcked(std::shared_ptr<Stanza> stanza);
            void ack(unsigned int handledStanzasCount);
            void continueAfterTLSEncrypted();
            void checkTrustOrFinish(const std::vector<Certificate::ref>& certificateChain, std::shared_ptr<CertificateVerificationError> error);
            void initiateShutdown(bool sendFooter);

        private:
            JID localJID;
            State state;
            std::shared_ptr<SessionStream> stream;
            IDNConverter* idnConverter = nullptr;
            CryptoProvider* crypto = nullptr;
            TimerFactory* timerFactory = nullptr;
            std::shared_ptr<Timer> streamShutdownTimeout;
            int sessionShutdownTimeoutInMilliseconds = 10000;
            bool allowPLAINOverNonTLS;
            bool useStreamCompression;
            UseTLS useTLS;
            bool useAcks;
            bool needSessionStart;
            bool needResourceBind;
            bool needAcking;
            bool rosterVersioningSupported;
            ClientAuthenticator* authenticator;
            std::shared_ptr<StanzaAckRequester> stanzaAckRequester_;
            std::shared_ptr<StanzaAckResponder> stanzaAckResponder_;
            std::shared_ptr<Swift::Error> error_;
            CertificateTrustChecker* certificateTrustChecker;
            bool singleSignOn;
            int authenticationPort;
    };
}
