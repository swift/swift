/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Slimber/Server.h>

#include <cassert>
#include <string>

#include <boost/bind.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/Elements/VCard.h>
#include <Swiften/LinkLocal/IncomingLinkLocalSession.h>
#include <Swiften/LinkLocal/LinkLocalConnector.h>
#include <Swiften/LinkLocal/LinkLocalServiceBrowser.h>
#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>
#include <Swiften/LinkLocal/OutgoingLinkLocalSession.h>
#include <Swiften/Network/BoostConnection.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/ConnectionServer.h>
#include <Swiften/Session/Session.h>
#include <Swiften/Session/SessionTracer.h>

#include <Limber/Server/ServerFromClientSession.h>
#include <Limber/Server/UserRegistry.h>

#include <Slimber/LinkLocalPresenceManager.h>
#include <Slimber/VCardCollection.h>

namespace Swift {

Server::Server(
        int clientConnectionPort,
        int linkLocalConnectionPort,
        LinkLocalServiceBrowser* linkLocalServiceBrowser,
        VCardCollection* vCardCollection,
        EventLoop* eventLoop) :
            linkLocalServiceRegistered(false),
            rosterRequested(false),
            clientConnectionPort(clientConnectionPort),
            linkLocalConnectionPort(linkLocalConnectionPort),
            linkLocalServiceBrowser(linkLocalServiceBrowser),
            vCardCollection(vCardCollection),
            eventLoop(eventLoop),
            presenceManager(nullptr),
            stopping(false) {
    linkLocalServiceBrowser->onServiceRegistered.connect(
            boost::bind(&Server::handleServiceRegistered, this, _1));
}

Server::~Server() {
    stop();
}

void Server::start() {
    assert(!serverFromClientConnectionServer);
    serverFromClientConnectionServer = BoostConnectionServer::create(
                    clientConnectionPort, boostIOServiceThread.getIOService(), eventLoop);
    serverFromClientConnectionServerSignalConnections.push_back(
        serverFromClientConnectionServer->onNewConnection.connect(
                boost::bind(&Server::handleNewClientConnection, this, _1)));
    serverFromClientConnectionServerSignalConnections.push_back(
        serverFromClientConnectionServer->onStopped.connect(
                boost::bind(&Server::handleClientConnectionServerStopped, this, _1)));

    assert(!serverFromNetworkConnectionServer);
    serverFromNetworkConnectionServer = BoostConnectionServer::create(
            linkLocalConnectionPort, boostIOServiceThread.getIOService(), eventLoop);
    serverFromNetworkConnectionServerSignalConnections.push_back(
        serverFromNetworkConnectionServer->onNewConnection.connect(
                boost::bind(&Server::handleNewLinkLocalConnection, this, _1)));
    serverFromNetworkConnectionServerSignalConnections.push_back(
        serverFromNetworkConnectionServer->onStopped.connect(
                boost::bind(&Server::handleLinkLocalConnectionServerStopped, this, _1)));

    assert(!presenceManager);
    presenceManager = new LinkLocalPresenceManager(linkLocalServiceBrowser);
    presenceManager->onRosterChanged.connect(
            boost::bind(&Server::handleRosterChanged, this, _1));
    presenceManager->onPresenceChanged.connect(
            boost::bind(&Server::handlePresenceChanged, this, _1));

    serverFromClientConnectionServer->start();
    serverFromNetworkConnectionServer->start();
}

void Server::stop() {
    stop(boost::optional<ServerError>());
}

void Server::stop(boost::optional<ServerError> e) {
    if (stopping) {
        return;
    }

    stopping = true;

    delete presenceManager;
    presenceManager = nullptr;

    if (serverFromClientSession) {
        serverFromClientSession->finishSession();
    }
    serverFromClientSession.reset();
    for (auto&& session : linkLocalSessions) {
        session->finishSession();
    }
    linkLocalSessions.clear();
    for (auto&& connector : connectors) {
        connector->cancel();
    }
    connectors.clear();
    tracers.clear();

    if (serverFromNetworkConnectionServer) {
        serverFromNetworkConnectionServer->stop();
        for (auto&& connection : serverFromNetworkConnectionServerSignalConnections) {
            connection.disconnect();
        }
        serverFromNetworkConnectionServerSignalConnections.clear();
        serverFromNetworkConnectionServer.reset();
    }
    if (serverFromClientConnectionServer) {
        serverFromClientConnectionServer->stop();
        for (auto&& connection : serverFromClientConnectionServerSignalConnections) {
            connection.disconnect();
        }
        serverFromClientConnectionServerSignalConnections.clear();
        serverFromClientConnectionServer.reset();
    }

    stopping = false;
    onStopped(e);
}

void Server::handleNewClientConnection(std::shared_ptr<Connection> connection) {
    if (serverFromClientSession) {
        connection->disconnect();
    }
    serverFromClientSession = std::shared_ptr<ServerFromClientSession>(
            new ServerFromClientSession(idGenerator.generateID(), connection,
                    &payloadParserFactories, &payloadSerializers, &xmlParserFactory, &userRegistry));
    serverFromClientSession->setAllowSASLEXTERNAL();
    serverFromClientSession->onSessionStarted.connect(
            boost::bind(&Server::handleSessionStarted, this));
    serverFromClientSession->onElementReceived.connect(
            boost::bind(&Server::handleElementReceived, this, _1,
                serverFromClientSession));
    serverFromClientSession->onSessionFinished.connect(
            boost::bind(&Server::handleSessionFinished, this,
            serverFromClientSession));
    //tracers.push_back(std::shared_ptr<SessionTracer>(
    //        new SessionTracer(serverFromClientSession)));
    serverFromClientSession->startSession();
}

void Server::handleSessionStarted() {
    onSelfConnected(true);
}

void Server::handleSessionFinished(std::shared_ptr<ServerFromClientSession>) {
    serverFromClientSession.reset();
    unregisterService();
    selfJID = JID();
    rosterRequested = false;
    onSelfConnected(false);
    lastPresence.reset();
}

void Server::unregisterService() {
    if (linkLocalServiceRegistered) {
        linkLocalServiceRegistered = false;
        linkLocalServiceBrowser->unregisterService();
    }
}

void Server::handleElementReceived(std::shared_ptr<ToplevelElement> element, std::shared_ptr<ServerFromClientSession> session) {
    std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element);
    if (!stanza) {
        return;
    }

    stanza->setFrom(session->getRemoteJID());
    if (!stanza->getTo().isValid()) {
        stanza->setTo(session->getLocalJID());
    }

    if (std::shared_ptr<Presence> presence = std::dynamic_pointer_cast<Presence>(stanza)) {
        if (presence->getType() == Presence::Available) {
            if (!linkLocalServiceRegistered) {
                linkLocalServiceRegistered = true;
                linkLocalServiceBrowser->registerService(
                        session->getRemoteJID().toBare().toString(),
                        linkLocalConnectionPort, getLinkLocalServiceInfo(presence));
            }
            else {
                linkLocalServiceBrowser->updateService(
                        getLinkLocalServiceInfo(presence));
            }
            lastPresence = presence;
        }
        else {
            unregisterService();
        }
    }
    else if (!stanza->getTo().isValid() || stanza->getTo() == session->getLocalJID() || stanza->getTo() == session->getRemoteJID().toBare()) {
        if (std::shared_ptr<IQ> iq = std::dynamic_pointer_cast<IQ>(stanza)) {
            if (iq->getPayload<RosterPayload>()) {
                if (iq->getType() == IQ::Get) {
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), presenceManager->getRoster()));
                    rosterRequested = true;
                    for (const auto& presence : presenceManager->getAllPresence()) {
                        session->sendElement(presence);
                    }
                }
                else {
                    session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::Forbidden, ErrorPayload::Cancel));
                }
            }
            if (std::shared_ptr<VCard> vcard = iq->getPayload<VCard>()) {
                if (iq->getType() == IQ::Get) {
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), vCardCollection->getOwnVCard()));
                }
                else {
                    vCardCollection->setOwnVCard(vcard);
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getID()));
                    if (lastPresence) {
                        linkLocalServiceBrowser->updateService(getLinkLocalServiceInfo(lastPresence));
                    }
                }
            }
            else {
                session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::FeatureNotImplemented, ErrorPayload::Cancel));
            }
        }
    }
    else {
        JID toJID = stanza->getTo();
        std::shared_ptr<Session> outgoingSession =
                getLinkLocalSessionForJID(toJID);
        if (outgoingSession) {
            outgoingSession->sendElement(stanza);
        }
        else {
            boost::optional<LinkLocalService> service =
                    presenceManager->getServiceForJID(toJID);
            if (service) {
                std::shared_ptr<LinkLocalConnector> connector =
                    getLinkLocalConnectorForJID(toJID);
                if (!connector) {
                    connector = std::shared_ptr<LinkLocalConnector>(
                            new LinkLocalConnector(
                                *service,
                                linkLocalServiceBrowser->getQuerier(),
                                BoostConnection::create(boostIOServiceThread.getIOService(), eventLoop)));
                    connector->onConnectFinished.connect(
                            boost::bind(&Server::handleConnectFinished, this, connector, _1));
                    connectors.push_back(connector);
                    connector->connect();
                }
                connector->queueElement(element);
            }
            else {
                session->sendElement(IQ::createError(
                        stanza->getFrom(), stanza->getID(),
                        ErrorPayload::RecipientUnavailable, ErrorPayload::Wait));
            }
        }
    }
}

void Server::handleNewLinkLocalConnection(std::shared_ptr<Connection> connection) {
    std::shared_ptr<IncomingLinkLocalSession> session(
            new IncomingLinkLocalSession(
                selfJID, connection,
                &payloadParserFactories, &payloadSerializers, &xmlParserFactory));
    registerLinkLocalSession(session);
}

void Server::handleLinkLocalSessionFinished(std::shared_ptr<Session> session) {
    //std::cout << "Link local session from " << session->getRemoteJID() << " ended" << std::endl;
    linkLocalSessions.erase(
            std::remove(linkLocalSessions.begin(), linkLocalSessions.end(), session),
            linkLocalSessions.end());
}

void Server::handleLinkLocalElementReceived(std::shared_ptr<ToplevelElement> element, std::shared_ptr<Session> session) {
    if (std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(element)) {
        JID fromJID = session->getRemoteJID();
        if (!presenceManager->getServiceForJID(fromJID.toBare())) {
            return; // TODO: Send error back
        }
        stanza->setFrom(fromJID);
        serverFromClientSession->sendElement(stanza);
    }
}

void Server::handleConnectFinished(std::shared_ptr<LinkLocalConnector> connector, bool error) {
    if (error) {
        SWIFT_LOG(warning) << "Error connecting" << std::endl;
        // TODO: Send back queued stanzas
    }
    else {
        std::shared_ptr<OutgoingLinkLocalSession> outgoingSession(
                new OutgoingLinkLocalSession(
                    selfJID, connector->getService().getJID(), connector->getConnection(),
                    &payloadParserFactories, &payloadSerializers, &xmlParserFactory));
        for (const auto& element : connector->getQueuedElements()) {
            outgoingSession->queueElement(element);
        }
        registerLinkLocalSession(outgoingSession);
    }
    connectors.erase(std::remove(connectors.begin(), connectors.end(), connector), connectors.end());
}

void Server::registerLinkLocalSession(std::shared_ptr<Session> session) {
    session->onSessionFinished.connect(
            boost::bind(&Server::handleLinkLocalSessionFinished, this, session));
    session->onElementReceived.connect(
            boost::bind(&Server::handleLinkLocalElementReceived, this, _1, session));
    linkLocalSessions.push_back(session);
    //tracers.push_back(std::make_shared<SessionTracer>(session));
    session->startSession();
}

std::shared_ptr<Session> Server::getLinkLocalSessionForJID(const JID& jid) {
    for (const auto& session : linkLocalSessions) {
        if (session->getRemoteJID() == jid) {
            return session;
        }
    }
    return std::shared_ptr<Session>();
}

std::shared_ptr<LinkLocalConnector> Server::getLinkLocalConnectorForJID(const JID& jid) {
    for (const auto& connector : connectors) {
        if (connector->getService().getJID() == jid) {
            return connector;
        }
    }
    return std::shared_ptr<LinkLocalConnector>();
}

void Server::handleServiceRegistered(const DNSSDServiceID& service) {
    selfJID = JID(service.getName());
}

void Server::handleRosterChanged(std::shared_ptr<RosterPayload> roster) {
    if (rosterRequested) {
        assert(serverFromClientSession);
        std::shared_ptr<IQ> iq = IQ::createRequest(
                IQ::Set, serverFromClientSession->getRemoteJID(),
                idGenerator.generateID(), roster);
        iq->setFrom(serverFromClientSession->getRemoteJID().toBare());
        serverFromClientSession->sendElement(iq);
    }
}

void Server::handlePresenceChanged(std::shared_ptr<Presence> presence) {
    if (rosterRequested) {
        serverFromClientSession->sendElement(presence);
    }
}

void Server::handleClientConnectionServerStopped(boost::optional<BoostConnectionServer::Error> e) {
    if (e) {
        if (*e == BoostConnectionServer::Conflict) {
            stop(ServerError(ServerError::C2SPortConflict));
        }
        else {
            stop(ServerError(ServerError::C2SError));
        }
    }
    else {
        stop();
    }
}

void Server::handleLinkLocalConnectionServerStopped(boost::optional<BoostConnectionServer::Error> e) {
    if (e) {
        if (*e == BoostConnectionServer::Conflict) {
            stop(ServerError(ServerError::LinkLocalPortConflict));
        }
        else {
            stop(ServerError(ServerError::LinkLocalError));
        }
    }
    else {
        stop();
    }
}

LinkLocalServiceInfo Server::getLinkLocalServiceInfo(std::shared_ptr<Presence> presence) {
    LinkLocalServiceInfo info;
    std::shared_ptr<VCard> vcard = vCardCollection->getOwnVCard();
    if (!vcard->getFamilyName().empty() || !vcard->getGivenName().empty()) {
        info.setFirstName(vcard->getGivenName());
        info.setLastName(vcard->getFamilyName());
    }
    else if (!vcard->getFullName().empty()) {
        std::pair<std::string,std::string> p = String::getSplittedAtFirst(vcard->getFullName(), ' ');
        info.setFirstName(p.first);
        info.setLastName(p.second);
    }
    if (!vcard->getNickname().empty()) {
        info.setNick(vcard->getNickname());
    }
    if (!vcard->getPreferredEMailAddress().address.empty()) {
        info.setEMail(vcard->getPreferredEMailAddress().address);
    }
    info.setMessage(presence->getStatus());
    switch (presence->getShow()) {
        case StatusShow::Online:
        case StatusShow::None:
        case StatusShow::FFC:
            info.setStatus(LinkLocalServiceInfo::Available);
            break;
        case StatusShow::Away:
        case StatusShow::XA:
            info.setStatus(LinkLocalServiceInfo::Away);
            break;
        case StatusShow::DND:
            info.setStatus(LinkLocalServiceInfo::DND);
            break;
    }
    info.setPort(linkLocalConnectionPort);
    return info;
}

}
