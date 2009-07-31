#include "Slimber/Server.h"

#include <string>
#include <boost/bind.hpp>

#include "Swiften/LinkLocal/LinkLocalConnector.h"
#include "Swiften/Network/Connection.h"
#include "Swiften/Session/SessionTracer.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Swiften/Session/SessionTracer.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/LinkLocal/OutgoingLinkLocalSession.h"
#include "Swiften/LinkLocal/IncomingLinkLocalSession.h"
#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Slimber/VCardCollection.h"
#include "Slimber/LinkLocalPresenceManager.h"
#include "Swiften/Server/ServerFromClientSession.h"

namespace Swift {

Server::Server(
		int clientConnectionPort, 
		int linkLocalConnectionPort, 
		LinkLocalServiceBrowser* linkLocalServiceBrowser,
		VCardCollection* vCardCollection) : 
			linkLocalServiceRegistered(false), 
			rosterRequested(false), 
			clientConnectionPort(clientConnectionPort), 
			linkLocalConnectionPort(linkLocalConnectionPort),
			linkLocalServiceBrowser(linkLocalServiceBrowser),
			vCardCollection(vCardCollection) {
	serverFromClientConnectionServer = 
			boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(
					clientConnectionPort, &boostIOServiceThread.getIOService()));
	serverFromClientConnectionServer->onNewConnection.connect(
			boost::bind(&Server::handleNewClientConnection, this, _1));
	serverFromClientConnectionServer->start();

	presenceManager = new LinkLocalPresenceManager(linkLocalServiceBrowser);
	presenceManager->onRosterChanged.connect(
			boost::bind(&Server::handleRosterChanged, this, _1));
	presenceManager->onPresenceChanged.connect(
			boost::bind(&Server::handlePresenceChanged, this, _1));

	linkLocalServiceBrowser->onServiceRegistered.connect(
			boost::bind(&Server::handleServiceRegistered, this, _1));

	/*
	serverFromNetworkConnectionServer = 
		boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(
			linkLocalConnectionPort, &boostIOServiceThread.getIOService()));
	serverFromNetworkConnectionServer->onNewConnection.connect(
			boost::bind(&Server::handleNewLinkLocalConnection, this, _1));
	serverFromNetworkConnectionServer->start();
	*/
}

Server::~Server() {
	delete presenceManager;
}

void Server::handleNewClientConnection(boost::shared_ptr<Connection> connection) {
	if (serverFromClientSession) {
		connection->disconnect();
	}
	serverFromClientSession = boost::shared_ptr<ServerFromClientSession>(
			new ServerFromClientSession(idGenerator.generateID(), connection, 
					&payloadParserFactories, &payloadSerializers, &userRegistry));
	serverFromClientSession->onSessionStarted.connect(
			boost::bind(&Server::handleSessionStarted, this));
	serverFromClientSession->onElementReceived.connect(
			boost::bind(&Server::handleElementReceived, this, _1, 
				serverFromClientSession));
	serverFromClientSession->onSessionFinished.connect(
			boost::bind(&Server::handleSessionFinished, this, 
			serverFromClientSession));
	tracers.push_back(boost::shared_ptr<SessionTracer>(
			new SessionTracer(serverFromClientSession)));
	serverFromClientSession->startSession();
}

void Server::handleSessionStarted() {
	onSelfConnected(true);
}

void Server::handleSessionFinished(boost::shared_ptr<ServerFromClientSession>) {
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

void Server::handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session) {
	boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element);
	if (!stanza) {
		return;
	}

	stanza->setFrom(session->getRemoteJID());
	if (!stanza->getTo().isValid()) {
		stanza->setTo(session->getLocalJID());
	}

	if (boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(stanza)) {
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
		if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza)) {
			if (iq->getPayload<RosterPayload>()) {
				if (iq->getType() == IQ::Get) {
					session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), presenceManager->getRoster()));
					rosterRequested = true;
					foreach(const boost::shared_ptr<Presence> presence, presenceManager->getAllPresence()) {
						session->sendElement(presence);
					}
				}
				else {
					session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), Error::Forbidden, Error::Cancel));
				}
			}
			if (boost::shared_ptr<VCard> vcard = iq->getPayload<VCard>()) {
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
				session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), Error::FeatureNotImplemented, Error::Cancel));
			}
		}
	}
	/*
	else {
		JID toJID = stanza->getTo();
		boost::shared_ptr<Session> outgoingSession = 
				getLinkLocalSessionForJID(toJID);
		if (outgoingSession) {
			outgoingSession->sendElement(stanza);
		}
		else {
			if (linkLocalServiceBrowser->hasItem(toJID)) {
				boost::shared_ptr<LinkLocalConnector> connector =
					getLinkLocalConnectorForJID(toJID);
				if (!connector) {
					connector = boost::shared_ptr<LinkLocalConnector>(
							new LinkLocalConnector(
								toJID, 
								linkLocalServiceBrowser->getHostname(toJID), 
								linkLocalServiceBrowser->getPort(toJID), 
								linkLocalServiceBrowser,
								boost::shared_ptr<BoostConnection>(new BoostConnection(&boostIOServiceThread.getIOService()))));
					connector->onConnectFinished.connect(
							boost::bind(&Server::handleConnectFinished, this, connector, _1));
					connectors_.push_back(connector);
					connector->connect();
				}
				connector->queueElement(element);
			}
			else {
				session->sendElement(IQ::createError(
						stanza->getFrom(), stanza->getID(), 
						Error::RecipientUnavailable, Error::Wait));
			}
		}
	}
	*/
}

/*
void Server::handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection) {
	boost::shared_ptr<IncomingLinkLocalSession> session(
			new IncomingLinkLocalSession(
				selfJID, connection, 
				&payloadParserFactories, &payloadSerializers));
	registerLinkLocalSession(session);
}

void Server::handleLinkLocalSessionFinished(boost::shared_ptr<Session> session) {
	std::cout << "Link local session from " << session->getRemoteJID() << " ended" << std::endl;
	linkLocalSessions_.erase(std::remove(linkLocalSessions_.begin(), linkLocalSessions_.end(), session), linkLocalSessions_.end());
}

void Server::handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session) {
	if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
		JID fromJID = session->getRemoteJID();
		if (!linkLocalServiceBrowser->hasItem(fromJID)) {
			return; // TODO: Queue
		}
		stanza->setFrom(fromJID);
		serverFromClientSession->sendElement(stanza);
	}
}

void Server::handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error) {
	if (error) {
		std::cerr << "Error connecting" << std::endl;
		// TODO: Send back queued stanzas
	}
	else {
		boost::shared_ptr<OutgoingLinkLocalSession> outgoingSession(
				new OutgoingLinkLocalSession(
					selfJID, connector->getRemoteJID(), connector->getConnection(),
					&payloadParserFactories, &payloadSerializers));
		foreach(const boost::shared_ptr<Element> element, connector->getQueuedElements()) {
			outgoingSession->queueElement(element);
		}
		registerLinkLocalSession(outgoingSession);
	}
	connectors_.erase(std::remove(connectors_.begin(), connectors_.end(), connector), connectors_.end());
}


void Server::registerLinkLocalSession(boost::shared_ptr<Session> session) {
	session->onSessionFinished.connect(boost::bind(&Server::handleLinkLocalSessionFinished, this, session));
	session->onElementReceived.connect(boost::bind(&Server::handleLinkLocalElementReceived, this, _1, session));
	linkLocalSessions_.push_back(session);
	//tracers.push_back(boost::shared_ptr<SessionTracer>(new SessionTracer(session)));
	session->startSession();
}

boost::shared_ptr<Session> Server::getLinkLocalSessionForJID(const JID& jid) {
	foreach(const boost::shared_ptr<Session> session, linkLocalSessions_) {
		if (session->getRemoteJID() == jid) {
			return session;
		}
	}
	return boost::shared_ptr<Session>();
}

boost::shared_ptr<LinkLocalConnector> Server::getLinkLocalConnectorForJID(const JID& jid) {
	foreach(const boost::shared_ptr<LinkLocalConnector> connector, connectors_) {
		if (connector->getRemoteJID() == jid) {
			return connector;
		}
	}
	return boost::shared_ptr<LinkLocalConnector>();
}
*/

void Server::handleServiceRegistered(const DNSSDServiceID& service) {
	selfJID = JID(service.getName());
}

void Server::handleRosterChanged(boost::shared_ptr<RosterPayload> roster) {
	if (rosterRequested) {
		assert(serverFromClientSession);
		boost::shared_ptr<IQ> iq = IQ::createRequest(
				IQ::Set, serverFromClientSession->getRemoteJID(), 
				idGenerator.generateID(), roster);
		iq->setFrom(serverFromClientSession->getRemoteJID().toBare());
		serverFromClientSession->sendElement(iq);
	}
}

void Server::handlePresenceChanged(boost::shared_ptr<Presence> presence) {
	if (rosterRequested) {
		serverFromClientSession->sendElement(presence);
	}
}

LinkLocalServiceInfo Server::getLinkLocalServiceInfo(boost::shared_ptr<Presence> presence) {
	LinkLocalServiceInfo info;
	boost::shared_ptr<VCard> vcard = vCardCollection->getOwnVCard();
	if (!vcard->getFamilyName().isEmpty() || !vcard->getGivenName().isEmpty()) {
		info.setFirstName(vcard->getGivenName());
		info.setLastName(vcard->getFamilyName());
	}
	else if (!vcard->getFullName().isEmpty()) {
		std::pair<String,String> p = vcard->getFullName().getSplittedAtFirst(' ');
		info.setFirstName(p.first);
		info.setLastName(p.second);
	}
	if (!vcard->getNickname().isEmpty()) {
		info.setNick(vcard->getNickname());
	}
	if (!vcard->getEMail().isEmpty()) {
		info.setEMail(vcard->getEMail());
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
