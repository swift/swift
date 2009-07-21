#include "Slimber/Server.h"

#include <string>
#include <boost/bind.hpp>

#include "Swiften/Session/SessionTracer.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Base/String.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"
#include "Swiften/LinkLocal/OutgoingLinkLocalSession.h"
#include "Swiften/LinkLocal/IncomingLinkLocalSession.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Slimber/VCardCollection.h"
#include "Swiften/Server/ServerFromClientSession.h"

namespace Swift {

Server::Server(int clientConnectionPort, int linkLocalConnectionPort, boost::shared_ptr<LinkLocalRoster> linkLocalRoster, boost::shared_ptr<DNSSDService> dnsSDService, VCardCollection* vCardCollection) : 
		dnsSDServiceRegistered_(false), 
		rosterRequested_(false), 
		clientConnectionPort_(clientConnectionPort), 
		linkLocalConnectionPort_(linkLocalConnectionPort),
		linkLocalRoster_(linkLocalRoster),
		dnsSDService_(dnsSDService),
		vCardCollection_(vCardCollection) {
	serverFromClientConnectionServer_ = 
			boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(
					clientConnectionPort, &boostIOServiceThread_.getIOService()));
	serverFromClientConnectionServer_->onNewConnection.connect(
			boost::bind(&Server::handleNewClientConnection, this, _1));
	serverFromClientConnectionServer_->start();

	serverFromNetworkConnectionServer_ = 
		boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(
			linkLocalConnectionPort, &boostIOServiceThread_.getIOService()));
	serverFromNetworkConnectionServer_->onNewConnection.connect(
			boost::bind(&Server::handleNewLinkLocalConnection, this, _1));
	serverFromNetworkConnectionServer_->start();

	dnsSDService_->onServiceRegistered.connect
			(boost::bind(&Server::handleServiceRegistered, this, _1));
	linkLocalRoster_->onRosterChanged.connect(
			boost::bind(&Server::handleRosterChanged, this, _1));
	linkLocalRoster_->onPresenceChanged.connect(
			boost::bind(&Server::handlePresenceChanged, this, _1));
	dnsSDService_->start();
}

void Server::handleNewClientConnection(boost::shared_ptr<Connection> c) {
	if (serverFromClientSession_) {
		c->disconnect();
	}
	serverFromClientSession_ = boost::shared_ptr<ServerFromClientSession>(new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &userRegistry_));
	serverFromClientSession_->onSessionStarted.connect(boost::bind(&Server::handleSessionStarted, this));
	serverFromClientSession_->onElementReceived.connect(boost::bind(&Server::handleElementReceived, this, _1, serverFromClientSession_));
	serverFromClientSession_->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, serverFromClientSession_));
	//tracers_.push_back(boost::shared_ptr<SessionTracer>(new SessionTracer(serverFromClientSession_)));
	serverFromClientSession_->startSession();
}

void Server::handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection) {
	boost::shared_ptr<IncomingLinkLocalSession> session(
			new IncomingLinkLocalSession(
				selfJID_, connection, 
				&payloadParserFactories_, &payloadSerializers_));
	registerLinkLocalSession(session);
}

void Server::handleServiceRegistered(const DNSSDService::Service& service) {
	std::cout << "Service registered " << service.name << " " << service.type << " " << service.domain << std::endl;
	selfJID_ = JID(service.name);
}

void Server::handleSessionStarted() {
	onSelfConnected(true);
}

void Server::handleSessionFinished(boost::shared_ptr<ServerFromClientSession>) {
	serverFromClientSession_.reset();
	unregisterService();
	selfJID_ = JID();
	rosterRequested_ = false;
	onSelfConnected(false);
	lastPresence_.reset();
}

void Server::handleLinkLocalSessionFinished(boost::shared_ptr<Session> session) {
	std::cout << "Link local session from " << session->getRemoteJID() << " ended" << std::endl;
	linkLocalSessions_.erase(std::remove(linkLocalSessions_.begin(), linkLocalSessions_.end(), session), linkLocalSessions_.end());
}

void Server::handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session) {
	if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
		JID fromJID = session->getRemoteJID();
		if (!linkLocalRoster_->hasItem(fromJID)) {
			return; // TODO: Queue
		}
		stanza->setFrom(fromJID);
		serverFromClientSession_->sendElement(stanza);
	}
}

void Server::unregisterService() {
	if (dnsSDServiceRegistered_) {
		dnsSDServiceRegistered_ = false;
		dnsSDService_->unregisterService();
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
			if (!dnsSDServiceRegistered_) {
				dnsSDServiceRegistered_ = true;
				dnsSDService_->registerService(session->getRemoteJID().toBare().toString(), linkLocalConnectionPort_, getLinkLocalServiceInfo(presence));
			}
			else {
				dnsSDService_->updateService(getLinkLocalServiceInfo(presence));
			}
			lastPresence_ = presence;
		}
		else {
			unregisterService();
		}
	}
	else if (!stanza->getTo().isValid() || stanza->getTo() == session->getLocalJID() || stanza->getTo() == session->getRemoteJID().toBare()) {
		if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza)) {
			if (iq->getPayload<RosterPayload>()) {
				if (iq->getType() == IQ::Get) {
					session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), linkLocalRoster_->getRoster()));
					rosterRequested_ = true;
					foreach(const boost::shared_ptr<Presence> presence, linkLocalRoster_->getAllPresence()) {
						session->sendElement(presence);
					}
				}
				else {
					session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), Error::Forbidden, Error::Cancel));
				}
			}
			if (boost::shared_ptr<VCard> vcard = iq->getPayload<VCard>()) {
				if (iq->getType() == IQ::Get) {
					session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), vCardCollection_->getOwnVCard()));
				}
				else {
					vCardCollection_->setOwnVCard(vcard);
					session->sendElement(IQ::createResult(iq->getFrom(), iq->getID()));
					if (lastPresence_) {
						dnsSDService_->updateService(getLinkLocalServiceInfo(lastPresence_));
					}
				}
			}
			else {
				session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), Error::FeatureNotImplemented, Error::Cancel));
			}
		}
	}
	else {
		JID toJID = stanza->getTo();
		boost::shared_ptr<Session> outgoingSession = 
				getLinkLocalSessionForJID(toJID);
		if (outgoingSession) {
			outgoingSession->sendElement(stanza);
		}
		else {
			if (linkLocalRoster_->hasItem(toJID)) {
				boost::shared_ptr<LinkLocalConnector> connector =
					getLinkLocalConnectorForJID(toJID);
				if (!connector) {
					connector = boost::shared_ptr<LinkLocalConnector>(
							new LinkLocalConnector(
								toJID, 
								linkLocalRoster_->getHostname(toJID), 
								linkLocalRoster_->getPort(toJID), 
								dnsSDService_,
								boost::shared_ptr<BoostConnection>(new BoostConnection(&boostIOServiceThread_.getIOService()))));
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
}

void Server::handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error) {
	if (error) {
		std::cerr << "Error connecting" << std::endl;
		// TODO: Send back queued stanzas
	}
	else {
		boost::shared_ptr<OutgoingLinkLocalSession> outgoingSession(
				new OutgoingLinkLocalSession(
					selfJID_, connector->getRemoteJID(), connector->getConnection(),
					&payloadParserFactories_, &payloadSerializers_));
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
	//tracers_.push_back(boost::shared_ptr<SessionTracer>(new SessionTracer(session)));
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

void Server::handleRosterChanged(boost::shared_ptr<RosterPayload> roster) {
	if (rosterRequested_) {
		boost::shared_ptr<IQ> iq = IQ::createRequest(IQ::Set, serverFromClientSession_->getRemoteJID(), idGenerator_.generateID(), roster);
		iq->setFrom(serverFromClientSession_->getRemoteJID().toBare());
		serverFromClientSession_->sendElement(iq);
	}
}

void Server::handlePresenceChanged(boost::shared_ptr<Presence> presence) {
	if (rosterRequested_) {
		serverFromClientSession_->sendElement(presence);
	}
}

LinkLocalServiceInfo Server::getLinkLocalServiceInfo(boost::shared_ptr<Presence> presence) {
	LinkLocalServiceInfo info;
	boost::shared_ptr<VCard> vcard = vCardCollection_->getOwnVCard();
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
	info.setPort(linkLocalConnectionPort_);
	return info;
}

}
