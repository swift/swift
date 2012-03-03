/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Entity/Entity.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Client/ClientError.h>
#include <Swiften/Client/ClientOptions.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/TLS/CertificateWithKey.h>

namespace Swift {
	class ChainedConnector;
	class Message;
	class Presence;
	class Error;
	class IQRouter;
	class TLSContextFactory;
	class ConnectionFactory;
	class Connection;
	class TimerFactory;
	class ClientSession;
	class StanzaChannel;
	class Stanza;
	class SessionStream;
	class CertificateTrustChecker;
	class NetworkFactories;
	class ClientSessionStanzaChannel;

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
	class CoreClient : public Entity {
		public: 
			/**
			 * Constructs a client for the given JID with the given password.
			 * The given eventLoop will be used to post events to.
			 */
			CoreClient(const JID& jid, const SafeByteArray& password, NetworkFactories* networkFactories);
			~CoreClient();

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

			void connect(const std::string& host);
			
			/**
			 * Sends a message.
			 */
			void sendMessage(boost::shared_ptr<Message>);

			/**
			 * Sends a presence stanza.
			 */
			void sendPresence(boost::shared_ptr<Presence>);

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
			boost::signal<void (const boost::optional<ClientError>&)> onDisconnected;

			/**
			 * Emitted when the client is connected and authenticated,
			 * and stanzas can be sent.
			 */
			boost::signal<void ()> onConnected;

			/**
			 * Emitted when the client receives data.
			 *
			 * This signal is emitted before the XML data is parsed,
			 * so this data is unformatted.
			 */
			boost::signal<void (const SafeByteArray&)> onDataRead;

			/**
			 * Emitted when the client sends data.
			 *
			 * This signal is emitted after the XML was serialized, and 
			 * is unformatted.
			 */
			boost::signal<void (const SafeByteArray&)> onDataWritten;

			/**
			 * Emitted when a message is received.
			 */
			boost::signal<void (boost::shared_ptr<Message>)> onMessageReceived;

			/**
			 * Emitted when a presence stanza is received.
			 */
			boost::signal<void (boost::shared_ptr<Presence>) > onPresenceReceived;

			/**
			 * Emitted when the server acknowledges receipt of a
			 * stanza (if acknowledgements are available).
			 *
			 * \see getStreamManagementEnabled()
			 */
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaAcked;

		protected:
			boost::shared_ptr<ClientSession> getSession() const {
				return session_;
			}

			NetworkFactories* getNetworkFactories() const {
				return networkFactories;
			}

			/**
			 * Called before onConnected signal is emmitted.
			 */
			virtual void handleConnected() {};

		private:
			void handleConnectorFinished(boost::shared_ptr<Connection>, boost::shared_ptr<Error> error);
			void handleStanzaChannelAvailableChanged(bool available);
			void handleSessionFinished(boost::shared_ptr<Error>);
			void handleNeedCredentials();
			void handleDataRead(const SafeByteArray&);
			void handleDataWritten(const SafeByteArray&);
			void handlePresenceReceived(boost::shared_ptr<Presence>);
			void handleMessageReceived(boost::shared_ptr<Message>);
			void handleStanzaAcked(boost::shared_ptr<Stanza>);
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
			boost::shared_ptr<ChainedConnector> connector_;
			std::vector<ConnectionFactory*> proxyConnectionFactories;
			boost::shared_ptr<Connection> connection_;
			boost::shared_ptr<SessionStream> sessionStream_;
			boost::shared_ptr<ClientSession> session_;
			CertificateWithKey::ref certificate_;
			bool disconnectRequested_;
			CertificateTrustChecker* certificateTrustChecker;
	};
}
