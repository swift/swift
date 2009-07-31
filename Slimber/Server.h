#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"

namespace Swift {
	class DNSSDServiceID;
	class String;
	class VCardCollection;
	class LinkLocalServiceBrowser;
	class LinkLocalPresenceManager;
	class BoostConnectionServer;
	class SessionTracer;
	class RosterPayload;
	class Presence;

	class Server {
		public:
			Server(
					int clientConnectionPort, 
					int linkLocalConnectionPort, 
					LinkLocalServiceBrowser* browser, 
					VCardCollection* vCardCollection);
			~Server();

			boost::signal<void (bool)> onSelfConnected;

		private:
			void handleNewClientConnection(boost::shared_ptr<Connection> c);
			void handleSessionStarted();
			void handleSessionFinished(boost::shared_ptr<ServerFromClientSession>);
			void handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session);
			void handleRosterChanged(boost::shared_ptr<RosterPayload> roster);
			void handlePresenceChanged(boost::shared_ptr<Presence> presence);
			void handleServiceRegistered(const DNSSDServiceID& service);
/*
			void handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection);
			void handleLinkLocalSessionFinished(boost::shared_ptr<Session> session);
			void handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session);
			void handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error);
			void registerLinkLocalSession(boost::shared_ptr<Session> session);
			boost::shared_ptr<Session> getLinkLocalSessionForJID(const JID& jid);
			boost::shared_ptr<LinkLocalConnector> getLinkLocalConnectorForJID(const JID& jid);
			*/

			void unregisterService();
			LinkLocalServiceInfo getLinkLocalServiceInfo(boost::shared_ptr<Presence> presence);

		private:
			class DummyUserRegistry : public UserRegistry {
				public:
					DummyUserRegistry() {}

					virtual bool isValidUserPassword(const JID&, const String&) const {
						return true;
					}
			};

		private:
			IDGenerator idGenerator;
			FullPayloadParserFactoryCollection payloadParserFactories;
			FullPayloadSerializerCollection payloadSerializers;
			BoostIOServiceThread boostIOServiceThread;
			DummyUserRegistry userRegistry;
			bool linkLocalServiceRegistered;
			bool rosterRequested;
			int clientConnectionPort;
			int linkLocalConnectionPort;
			LinkLocalServiceBrowser* linkLocalServiceBrowser;
			VCardCollection* vCardCollection;
			LinkLocalPresenceManager* presenceManager;
			boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer;
			boost::shared_ptr<ServerFromClientSession> serverFromClientSession;
			boost::shared_ptr<Presence> lastPresence;
			JID selfJID;
			/*
			boost::shared_ptr<BoostConnectionServer> serverFromNetworkConnectionServer_;
			std::vector< boost::shared_ptr<Session> > linkLocalSessions_;
			std::vector< boost::shared_ptr<LinkLocalConnector> > connectors_;
*/
			std::vector< boost::shared_ptr<SessionTracer> > tracers;
	};
}
