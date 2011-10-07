/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <vector>

#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Limber/Server/UserRegistry.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"
#include "Limber/Server/ServerFromClientSession.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Slimber/ServerError.h"

namespace Swift {
	class DNSSDServiceID;
	
	class VCardCollection;
	class LinkLocalConnector;
	class LinkLocalServiceBrowser;
	class LinkLocalPresenceManager;
	class BoostConnectionServer;
	class SessionTracer;
	class RosterPayload;
	class Presence;
	class EventLoop;

	class Server {
		public:
			Server(
					int clientConnectionPort, 
					int linkLocalConnectionPort, 
					LinkLocalServiceBrowser* browser, 
					VCardCollection* vCardCollection,
					EventLoop* eventLoop);
			~Server();

			void start();
			void stop();

			int getLinkLocalPort() const {
				return linkLocalConnectionPort;
			}

			int getClientToServerPort() const {
				return clientConnectionPort;
			}

			boost::signal<void (bool)> onSelfConnected;
			boost::signal<void (boost::optional<ServerError>)> onStopped;

		private:
			void stop(boost::optional<ServerError>);

			void handleNewClientConnection(boost::shared_ptr<Connection> c);
			void handleSessionStarted();
			void handleSessionFinished(boost::shared_ptr<ServerFromClientSession>);
			void handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session);
			void handleRosterChanged(boost::shared_ptr<RosterPayload> roster);
			void handlePresenceChanged(boost::shared_ptr<Presence> presence);
			void handleServiceRegistered(const DNSSDServiceID& service);
			void handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection);
			void handleLinkLocalSessionFinished(boost::shared_ptr<Session> session);
			void handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session);
			void handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error);
			void handleClientConnectionServerStopped(
					boost::optional<BoostConnectionServer::Error>);
			void handleLinkLocalConnectionServerStopped(
					boost::optional<BoostConnectionServer::Error>);
			boost::shared_ptr<Session> getLinkLocalSessionForJID(const JID& jid);
			boost::shared_ptr<LinkLocalConnector> getLinkLocalConnectorForJID(const JID& jid);
			void registerLinkLocalSession(boost::shared_ptr<Session> session);
			void unregisterService();
			LinkLocalServiceInfo getLinkLocalServiceInfo(boost::shared_ptr<Presence> presence);

		private:
			class DummyUserRegistry : public UserRegistry {
				public:
					DummyUserRegistry() {}

					virtual bool isValidUserPassword(const JID&, const SafeByteArray&) const {
						return true;
					}
			};

		private:
			IDGenerator idGenerator;
			FullPayloadParserFactoryCollection payloadParserFactories;
			FullPayloadSerializerCollection payloadSerializers;
			BoostIOServiceThread boostIOServiceThread;
			DummyUserRegistry userRegistry;
			PlatformXMLParserFactory xmlParserFactory;
			bool linkLocalServiceRegistered;
			bool rosterRequested;
			int clientConnectionPort;
			int linkLocalConnectionPort;
			LinkLocalServiceBrowser* linkLocalServiceBrowser;
			VCardCollection* vCardCollection;
			EventLoop* eventLoop;
			LinkLocalPresenceManager* presenceManager;
			bool stopping;
			boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer;
			std::vector<boost::bsignals::connection> serverFromClientConnectionServerSignalConnections;
			boost::shared_ptr<ServerFromClientSession> serverFromClientSession;
			boost::shared_ptr<Presence> lastPresence;
			JID selfJID;
			boost::shared_ptr<BoostConnectionServer> serverFromNetworkConnectionServer;
			std::vector<boost::bsignals::connection> serverFromNetworkConnectionServerSignalConnections;
			std::vector< boost::shared_ptr<Session> > linkLocalSessions;
			std::vector< boost::shared_ptr<LinkLocalConnector> > connectors;
			std::vector< boost::shared_ptr<SessionTracer> > tracers;
	};
}
