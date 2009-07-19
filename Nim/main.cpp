#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Session/SessionTracer.h"
#include "Swiften/Network/BoostConnectionFactory.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Base/String.h"
#include "Swiften/Server/UserRegistry.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Elements/Element.h"
#include "Swiften/LinkLocal/LinkLocalConnector.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"
#include "Swiften/LinkLocal/OutgoingLinkLocalSession.h"
#include "Swiften/LinkLocal/IncomingLinkLocalSession.h"
#include "Swiften/LinkLocal/DNSSDService.h"
#include "Swiften/LinkLocal/AppleDNSSDService.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;


class DummyUserRegistry : public UserRegistry {
	public:
		DummyUserRegistry() {
		}

		virtual bool isValidUserPassword(const JID&, const String&) const {
			return true;
		}
};

class Server {
	public:
		Server(int clientConnectionPort, int linkLocalConnectionPort) : 
				dnsSDServiceRegistered_(false), 
				rosterRequested_(false), 
				clientConnectionPort_(clientConnectionPort), 
				linkLocalConnectionPort_(linkLocalConnectionPort) {
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

			dnsSDService_ = boost::shared_ptr<AppleDNSSDService>(
					new AppleDNSSDService());
			dnsSDService_->onServiceRegistered.connect
					(boost::bind(&Server::handleServiceRegistered, this, _1));
			linkLocalRoster_ = boost::shared_ptr<LinkLocalRoster>(
					new LinkLocalRoster(dnsSDService_));
			linkLocalRoster_->onRosterChanged.connect(
					boost::bind(&Server::handleRosterChanged, this, _1));
			linkLocalRoster_->onPresenceChanged.connect(
					boost::bind(&Server::handlePresenceChanged, this, _1));
			dnsSDService_->start();
		}

	private:
		void handleNewClientConnection(boost::shared_ptr<Connection> c) {
			if (serverFromClientSession_) {
				c->disconnect();
			}
			serverFromClientSession_ = boost::shared_ptr<ServerFromClientSession>(new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &userRegistry_));
			serverFromClientSession_->onElementReceived.connect(boost::bind(&Server::handleElementReceived, this, _1, serverFromClientSession_));
			serverFromClientSession_->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, serverFromClientSession_));
			tracers_.push_back(boost::shared_ptr<SessionTracer>(new SessionTracer(serverFromClientSession_)));
			serverFromClientSession_->startSession();
		}

		void handleNewLinkLocalConnection(boost::shared_ptr<Connection> connection) {
			boost::shared_ptr<IncomingLinkLocalSession> session(
					new IncomingLinkLocalSession(
						selfJID_, connection, 
						&payloadParserFactories_, &payloadSerializers_));
			registerLinkLocalSession(session);
		}
		
		void handleServiceRegistered(const DNSSDService::Service& service) {
			std::cout << "Service registered " << service.name << " " << service.type << " " << service.domain << std::endl;
			selfJID_ = JID(service.name);
		}

		void handleSessionFinished(boost::shared_ptr<ServerFromClientSession>) {
			serverFromClientSession_.reset();
			unregisterService();
			selfJID_ = JID();
			rosterRequested_ = false;
		}

		void handleLinkLocalSessionFinished(boost::shared_ptr<Session> session) {
			std::cout << "Link local session from " << session->getRemoteJID() << " ended" << std::endl;
			linkLocalSessions_.erase(std::remove(linkLocalSessions_.begin(), linkLocalSessions_.end(), session), linkLocalSessions_.end());
		}

		void handleLinkLocalElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<Session> session) {
			if (boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(element)) {
				JID fromJID = session->getRemoteJID();
				if (!linkLocalRoster_->hasItem(fromJID)) {
					return; // TODO: Queue
				}
				stanza->setFrom(fromJID);
				serverFromClientSession_->sendElement(stanza);
			}
		}

		void unregisterService() {
			if (dnsSDServiceRegistered_) {
				dnsSDServiceRegistered_ = false;
				dnsSDService_->unregisterService();
			}
		}

		void handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session) {
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
					if (iq->getPayload<VCard>()) {
						if (iq->getType() == IQ::Get) {
							boost::shared_ptr<VCard> vcard(new VCard());
							vcard->setNickname(iq->getFrom().getNode());
							session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), vcard));
						}
						else {
							session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), Error::Forbidden, Error::Cancel));
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

		void handleConnectFinished(boost::shared_ptr<LinkLocalConnector> connector, bool error) {
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


		void registerLinkLocalSession(boost::shared_ptr<Session> session) {
			session->onSessionFinished.connect(boost::bind(&Server::handleLinkLocalSessionFinished, this, session));
			session->onElementReceived.connect(boost::bind(&Server::handleLinkLocalElementReceived, this, _1, session));
			linkLocalSessions_.push_back(session);
			tracers_.push_back(boost::shared_ptr<SessionTracer>(new SessionTracer(session)));
			session->startSession();
		}

		boost::shared_ptr<Session> getLinkLocalSessionForJID(const JID& jid) {
			foreach(const boost::shared_ptr<Session> session, linkLocalSessions_) {
				if (session->getRemoteJID() == jid) {
					return session;
				}
			}
			return boost::shared_ptr<Session>();
		}

		boost::shared_ptr<LinkLocalConnector> getLinkLocalConnectorForJID(const JID& jid) {
			foreach(const boost::shared_ptr<LinkLocalConnector> connector, connectors_) {
				if (connector->getRemoteJID() == jid) {
					return connector;
				}
			}
			return boost::shared_ptr<LinkLocalConnector>();
		}

		void handleRosterChanged(boost::shared_ptr<RosterPayload> roster) {
			if (rosterRequested_) {
				boost::shared_ptr<IQ> iq = IQ::createRequest(IQ::Set, serverFromClientSession_->getRemoteJID(), idGenerator_.generateID(), roster);
				iq->setFrom(serverFromClientSession_->getRemoteJID().toBare());
				serverFromClientSession_->sendElement(iq);
			}
		}

		void handlePresenceChanged(boost::shared_ptr<Presence> presence) {
			if (rosterRequested_) {
				serverFromClientSession_->sendElement(presence);
			}
		}

		LinkLocalServiceInfo getLinkLocalServiceInfo(boost::shared_ptr<Presence> presence) {
			LinkLocalServiceInfo info;
			info.setFirstName("Remko");
			info.setLastName("Tron\xc3\xa7on");
			info.setEMail("email@example.com");
			info.setJID(JID("jid@example.com"));
			info.setMessage(presence->getStatus());
			info.setNick("Remko");
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

	private:
		IDGenerator idGenerator_;
		BoostIOServiceThread boostIOServiceThread_;
		DummyUserRegistry userRegistry_;
		boost::shared_ptr<AppleDNSSDService> dnsSDService_;
		boost::shared_ptr<LinkLocalRoster> linkLocalRoster_;
		boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
		boost::shared_ptr<ServerFromClientSession> serverFromClientSession_;
		boost::shared_ptr<BoostConnectionServer> serverFromNetworkConnectionServer_;
		std::vector< boost::shared_ptr<SessionTracer> > tracers_;
		std::vector< boost::shared_ptr<Session> > linkLocalSessions_;
		std::vector< boost::shared_ptr<LinkLocalConnector> > connectors_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
		bool dnsSDServiceRegistered_;
		bool rosterRequested_;
		int clientConnectionPort_;
		int linkLocalConnectionPort_;
		JID selfJID_;
};

int main() {
	SimpleEventLoop eventLoop;
	Server server(5222, 5562);
	eventLoop.run();
	return 0;
}
