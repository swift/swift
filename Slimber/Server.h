#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"
#include "Swiften/Session/SessionTracer.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/LinkLocal/LinkLocalConnector.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

namespace Swift {
	class VCardCollection;

	class Server {
		public:
			Server(int clientConnectionPort, int linkLocalConnectionPort, boost::shared_ptr<LinkLocalRoster>, boost::shared_ptr<DNSSDService> dnsSDService, VCardCollection* vCardCollection);

			boost::signal<void (bool)> onSelfConnected;

		private:
			void handleNewClientConnection(boost::shared_ptr<Connection> c);
			void handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection);
			void handleServiceRegistered(const DNSSDServiceID& service);
			void handleSessionStarted();
			void handleSessionFinished(boost::shared_ptr<ServerFromClientSession>);
			void handleLinkLocalSessionFinished(boost::shared_ptr<Session> session);
			void handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session);
			void unregisterService();
			void handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session);
			void handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error);
			void registerLinkLocalSession(boost::shared_ptr<Session> session);
			boost::shared_ptr<Session> getLinkLocalSessionForJID(const JID& jid);
			boost::shared_ptr<LinkLocalConnector> getLinkLocalConnectorForJID(const JID& jid);
			void handleRosterChanged(boost::shared_ptr<RosterPayload> roster);
			void handlePresenceChanged(boost::shared_ptr<Presence> presence);
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
			IDGenerator idGenerator_;
			BoostIOServiceThread boostIOServiceThread_;
			DummyUserRegistry userRegistry_;
			bool dnsSDServiceRegistered_;
			bool rosterRequested_;
			int clientConnectionPort_;
			int linkLocalConnectionPort_;
			boost::shared_ptr<LinkLocalRoster> linkLocalRoster_;
			boost::shared_ptr<DNSSDService> dnsSDService_;
			VCardCollection* vCardCollection_;
			boost::shared_ptr<Presence> lastPresence_;
			boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
			boost::shared_ptr<ServerFromClientSession> serverFromClientSession_;
			boost::shared_ptr<BoostConnectionServer> serverFromNetworkConnectionServer_;
			std::vector< boost::shared_ptr<SessionTracer> > tracers_;
			std::vector< boost::shared_ptr<Session> > linkLocalSessions_;
			std::vector< boost::shared_ptr<LinkLocalConnector> > connectors_;
			FullPayloadParserFactoryCollection payloadParserFactories_;
			FullPayloadSerializerCollection payloadSerializers_;
			JID selfJID_;
	};
}
