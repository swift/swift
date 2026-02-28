/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Client/ClientError.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Entity/Entity.h>
#include <Swiften/JID/JID.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
    class CertificateTrustChecker;
    class ChainedConnector;
    class ClientSession;
    class ClientSessionStanzaChannel;
    class Connection;
    class ConnectionFactory;
    class Error;
    class IQRouter;
    class Message;
    class NetworkFactories;
    class Presence;
    class SessionStream;
    class Stanza;
    class StanzaChannel;

    /**
     * The central class for communicating with an XMPP server.
     *
     * This class is responsible for setting up the connection with the XMPP
     * server, authenticating, and initializing the session.
     *
     * This class can be used directly in your application, although the Client
     * subclass provides more functionality and interfaces, and is better suited
     * for most needs.
     */
    class SWIFTEN_API CoreClient : public Entity {
        public:
            /**
             * Constructs a client for the given JID with the given password.
             */
            CoreClient(const JID& jid, const SafeByteArray& password, NetworkFactories* networkFactories);
            virtual ~CoreClient();

            /**
             * Set a client certificate to use for strong authentication with the server.
             * Ensure that it is of the correct type for the TLS engine in use.
             * This means, largely, PKCS12Certificate for OpenSSL and CAPICertificate for CAPI.
             */
            void setCertificate(CertificateWithKey::ref certificate);

            /**
             * Connects the client to the server.
             *
             * After the connection is established, the client will set
             * initialize the stream and authenticate.
             */
            void connect(const ClientOptions& = ClientOptions());

            /**
             * Disconnects the client from the server.
             */
            void disconnect();

            /**
             * Sends a message.
             */
            void sendMessage(std::shared_ptr<Message>);

            /**
             * Sends a presence stanza.
             */
            void sendPresence(std::shared_ptr<Presence>);

            /**
             * Sends raw, unchecked data.
             */
            void sendData(const std::string& data);

            /**
             * Returns the IQ router for this client.
             */
            IQRouter* getIQRouter() const {
                return iqRouter_;
            }

            /**
             * Checks whether the client is connected to the server,
             * and stanzas can be sent.
             */
            bool isAvailable() const;

            /**
             * Checks whether the client is active.
             *
             * A client is active when it is connected or connecting to the server.
             */
            bool isActive() const;

            /**
             * Returns the JID of the client.
             * After the session was initialized, this returns the bound JID.
             */
            const JID& getJID() const;

            /**
             * Checks whether stream management is enabled.
             *
             * If stream management is enabled, onStanzaAcked will be
             * emitted when a stanza is received by the server.
             *
             * \see onStanzaAcked
             */
            bool getStreamManagementEnabled() const;

            /**
             * Checks whether stream encryption (TLS) is currently active.
             */
            bool isStreamEncrypted() const;

            StanzaChannel* getStanzaChannel() const;

            /**
             * Sets the certificate trust checker.
             *
             * This checker will be called when the server sends a
             * TLS certificate that does not validate. If the trust checker
             * says the certificate is trusted, then connecting will proceed;
             * if not, the connection will end with an error.
             */
            void setCertificateTrustChecker(CertificateTrustChecker*);

        public:
            /**
             * Emitted when the client was disconnected from the network.
             *
             * If the connection was due to a non-recoverable error, the type
             * of error will be passed as a parameter.
             */
            boost::signals2::signal<void (const boost::optional<ClientError>&)> onDisconnected;

            /**
             * Emitted when the client is connected and authenticated,
             * and stanzas can be sent.
             */
            boost::signals2::signal<void ()> onConnected;

            /**
             * Emitted when the client receives data.
             *
             * This signal is emitted before the XML data is parsed,
             * so this data is unformatted.
             */
            boost::signals2::signal<void (const SafeByteArray&)> onDataRead;

            /**
             * Emitted when the client sends data.
             *
             * This signal is emitted after the XML was serialized, and
             * is unformatted.
             */
            boost::signals2::signal<void (const SafeByteArray&)> onDataWritten;

            /**
             * Emitted when a message is received.
             */
            boost::signals2::signal<void (std::shared_ptr<Message>)> onMessageReceived;

            /**
             * Emitted when a presence stanza is received.
             */
            boost::signals2::signal<void (std::shared_ptr<Presence>) > onPresenceReceived;

            /**
             * Emitted when the server acknowledges receipt of a
             * stanza (if acknowledgements are available).
             *
             * \see getStreamManagementEnabled()
             */
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaAcked;

        protected:
            std::shared_ptr<ClientSession> getSession() const {
                return session_;
            }

            NetworkFactories* getNetworkFactories() const {
                return networkFactories;
            }

            /**
             * Called before onConnected signal is emmitted.
             */
            virtual void handleConnected() {}

        private:
            void handleConnectorFinished(std::shared_ptr<Connection>, std::shared_ptr<Error> error);
            void handleStanzaChannelAvailableChanged(bool available);
            void handleSessionFinished(std::shared_ptr<Error>);
            void handleNeedCredentials();
            void handleDataRead(const SafeByteArray&);
            void handleDataWritten(const SafeByteArray&);
            void handlePresenceReceived(std::shared_ptr<Presence>);
            void handleMessageReceived(std::shared_ptr<Message>);
            void handleStanzaAcked(std::shared_ptr<Stanza>);
            void purgePassword();
            void bindSessionToStream();

            void resetConnector();
            void resetSession();
            void forceReset();

        private:
            JID jid_;
            SafeByteArray password_;
            NetworkFactories* networkFactories;
            ClientSessionStanzaChannel* stanzaChannel_;
            IQRouter* iqRouter_;
            ClientOptions options;
            std::shared_ptr<ChainedConnector> connector_;
            std::vector<ConnectionFactory*> proxyConnectionFactories;
            std::shared_ptr<Connection> connection_;
            std::shared_ptr<SessionStream> sessionStream_;
            std::shared_ptr<ClientSession> session_;
            CertificateWithKey::ref certificate_;
            bool disconnectRequested_;
            CertificateTrustChecker* certificateTrustChecker;
    };
}
