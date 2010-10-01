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
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

namespace Swift {
	class IQRouter;
	class TLSLayerFactory;
	class ConnectionFactory;
	class TimerFactory;
	class ClientSession;
	class BasicSessionStream;

	class Client : public StanzaChannel, public boost::bsignals::trackable {
		public:
			Client(const JID& jid, const String& password);
			~Client();

			void setCertificate(const String& certificate);

			void connect();
			void connect(const JID& jid);
			void connect(const String& host);
			void disconnect();
			
			bool isAvailable();

			bool getStreamManagementEnabled() const;

			virtual void sendIQ(boost::shared_ptr<IQ>);
			virtual void sendMessage(boost::shared_ptr<Message>);
			virtual void sendPresence(boost::shared_ptr<Presence>);

			IQRouter* getIQRouter() const {
				return iqRouter_;
			}

			JID getBoundJID();

		public:
			boost::signal<void (const ClientError&)> onError;
			boost::signal<void ()> onConnected;
			boost::signal<void (const String&)> onDataRead;
			boost::signal<void (const String&)> onDataWritten;

		private:
			void handleConnectorFinished(boost::shared_ptr<Connection>, Connector::ref);
			void handleSessionInitialized();
			void send(boost::shared_ptr<Stanza>);
			virtual String getNewIQID();
			void handleStanza(boost::shared_ptr<Stanza>);
			void handleSessionFinished(boost::shared_ptr<Error>);
			void handleNeedCredentials();
			void handleDataRead(const String&);
			void handleDataWritten(const String&);
			void handleStanzaAcked(boost::shared_ptr<Stanza>);

			void closeConnection();

		private:
			PlatformDomainNameResolver resolver_;
			JID jid_;
			String password_;
			IDGenerator idGenerator_;
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
	};
}
