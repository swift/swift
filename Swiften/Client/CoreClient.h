/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Network/PlatformDomainNameResolver.h"
#include "Swiften/Network/Connector.h"
#include "Swiften/Base/Error.h"
#include "Swiften/Client/ClientSession.h"
#include "Swiften/Client/ClientError.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Client/ClientSessionStanzaChannel.h"

namespace Swift {
	class IQRouter;
	class TLSLayerFactory;
	class ConnectionFactory;
	class TimerFactory;
	class ClientSession;
	class BasicSessionStream;

	/**
	 * The central class for communicating with an XMPP server.
	 *
	 * This class is responsible for setting up the connection with the XMPP server, authenticating, and
	 * initializing the session.
	 *
	 * This class can be used directly in your application, although the Client subclass provides more
	 * functionality and interfaces, and is better suited for most needs.
	 */
	class CoreClient  {
		public:
			CoreClient(const JID& jid, const String& password);
			~CoreClient();

			void setCertificate(const String& certificate);

			void connect();
			void connect(const JID& jid);
			void connect(const String& host);
			void disconnect();
			
			void sendMessage(boost::shared_ptr<Message>);
			void sendPresence(boost::shared_ptr<Presence>);

			IQRouter* getIQRouter() const {
				return iqRouter_;
			}

			StanzaChannel* getStanzaChannel() const {
				return stanzaChannel_;
			}

			bool isAvailable() const {
				return stanzaChannel_->isAvailable();
			}

			/**
			 * Returns the JID of the client. 
			 * After the session was initialized, this returns the bound JID.
			 */
			const JID& getJID() const {
				if (session_) {
					return session_->getLocalJID();
				}
				else {
					return jid_;
				}
			}

		public:
			boost::signal<void (const ClientError&)> onError;
			boost::signal<void ()> onConnected;
			boost::signal<void (const String&)> onDataRead;
			boost::signal<void (const String&)> onDataWritten;

			boost::signal<void (boost::shared_ptr<Message>)> onMessageReceived;
			boost::signal<void (boost::shared_ptr<Presence>) > onPresenceReceived;
			boost::signal<void (boost::shared_ptr<Stanza>)> onStanzaAcked;

		private:
			void handleConnectorFinished(boost::shared_ptr<Connection>);
			void handleStanzaChannelAvailableChanged(bool available);
			void handleSessionFinished(boost::shared_ptr<Error>);
			void handleNeedCredentials();
			void handleDataRead(const String&);
			void handleDataWritten(const String&);

		private:
			PlatformDomainNameResolver resolver_;
			JID jid_;
			String password_;
			ClientSessionStanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;
			Connector::ref connector_;
			ConnectionFactory* connectionFactory_;
			TimerFactory* timerFactory_;
			TLSLayerFactory* tlsLayerFactory_;
			FullPayloadParserFactoryCollection payloadParserFactories_;
			FullPayloadSerializerCollection payloadSerializers_;
			boost::shared_ptr<Connection> connection_;
			boost::shared_ptr<BasicSessionStream> sessionStream_;
			boost::shared_ptr<ClientSession> session_;
			String certificate_;
			bool disconnectRequested_;
	};
}
