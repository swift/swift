/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/JID/JID.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

#include <Limber/Server/ServerFromClientSession.h>
#include <Limber/Server/UserRegistry.h>

#include <Slimber/ServerError.h>

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
                    unsigned short clientConnectionPort,
                    unsigned short linkLocalConnectionPort,
                    LinkLocalServiceBrowser* browser,
                    VCardCollection* vCardCollection,
                    EventLoop* eventLoop);
            ~Server();

            void start();
            void stop();

            unsigned short getLinkLocalPort() const {
                return linkLocalConnectionPort;
            }

            unsigned short getClientToServerPort() const {
                return clientConnectionPort;
            }

            boost::signals2::signal<void (bool)> onSelfConnected;
            boost::signals2::signal<void (boost::optional<ServerError>)> onStopped;

        private:
            void stop(boost::optional<ServerError>);

            void handleNewClientConnection(std::shared_ptr<Connection> c);
            void handleSessionStarted();
            void handleSessionFinished(std::shared_ptr<ServerFromClientSession>);
            void handleElementReceived(std::shared_ptr<ToplevelElement> element, std::shared_ptr<ServerFromClientSession> session);
            void handleRosterChanged(std::shared_ptr<RosterPayload> roster);
            void handlePresenceChanged(std::shared_ptr<Presence> presence);
            void handleServiceRegistered(const DNSSDServiceID& service);
            void handleNewLinkLocalConnection(std::shared_ptr<Connection> connection);
            void handleLinkLocalSessionFinished(std::shared_ptr<Session> session);
            void handleLinkLocalElementReceived(std::shared_ptr<ToplevelElement> element, std::shared_ptr<Session> session);
            void handleConnectFinished(std::shared_ptr<LinkLocalConnector> connector, bool error);
            void handleClientConnectionServerStopped(
                    boost::optional<BoostConnectionServer::Error>);
            void handleLinkLocalConnectionServerStopped(
                    boost::optional<BoostConnectionServer::Error>);
            std::shared_ptr<Session> getLinkLocalSessionForJID(const JID& jid);
            std::shared_ptr<LinkLocalConnector> getLinkLocalConnectorForJID(const JID& jid);
            void registerLinkLocalSession(std::shared_ptr<Session> session);
            void unregisterService();
            LinkLocalServiceInfo getLinkLocalServiceInfo(std::shared_ptr<Presence> presence);

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
            unsigned short clientConnectionPort;
            unsigned short linkLocalConnectionPort;
            LinkLocalServiceBrowser* linkLocalServiceBrowser;
            VCardCollection* vCardCollection;
            EventLoop* eventLoop;
            LinkLocalPresenceManager* presenceManager;
            bool stopping;
            std::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer;
            std::vector<boost::signals2::connection> serverFromClientConnectionServerSignalConnections;
            std::shared_ptr<ServerFromClientSession> serverFromClientSession;
            std::shared_ptr<Presence> lastPresence;
            JID selfJID;
            std::shared_ptr<BoostConnectionServer> serverFromNetworkConnectionServer;
            std::vector<boost::signals2::connection> serverFromNetworkConnectionServerSignalConnections;
            std::vector< std::shared_ptr<Session> > linkLocalSessions;
            std::vector< std::shared_ptr<LinkLocalConnector> > connectors;
            std::vector< std::shared_ptr<SessionTracer> > tracers;
    };
}
