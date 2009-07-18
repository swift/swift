#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

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
#include "Swiften/Elements/Stanza.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/LinkLocal/LinkLocalRoster.h"
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
		Server(int clientConnectionPort, int linkLocalConnectionPort) : dnsSDServiceRegistered_(false), rosterRequested_(false), clientConnectionPort_(clientConnectionPort), linkLocalConnectionPort_(linkLocalConnectionPort) {
			serverFromClientConnectionServer_ = boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(clientConnectionPort, &boostIOServiceThread_.getIOService()));
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewClientConnection, this, _1));
			serverFromClientConnectionServer_->start();

			serverFromNetworkConnectionServer_ = boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(linkLocalConnectionPort, &boostIOServiceThread_.getIOService()));
			serverFromNetworkConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewLinkLocalConnection, this, _1));
			serverFromNetworkConnectionServer_->start();

			dnsSDService_ = boost::shared_ptr<AppleDNSSDService>(new AppleDNSSDService());
			dnsSDService_->onServiceRegistered.connect(boost::bind(&Server::handleServiceRegistered, this, _1));

			linkLocalRoster_ = boost::shared_ptr<LinkLocalRoster>(new LinkLocalRoster(dnsSDService_));
			linkLocalRoster_->onRosterChanged.connect(boost::bind(&Server::handleRosterChanged, this, _1));
			linkLocalRoster_->onPresenceChanged.connect(boost::bind(&Server::handlePresenceChanged, this, _1));
			dnsSDService_->start();
		}

	private:
		void handleNewClientConnection(boost::shared_ptr<Connection> c) {
			if (serverFromClientSession_) {
				c->disconnect();
			}
			serverFromClientSession_ = boost::shared_ptr<ServerFromClientSession>(new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &userRegistry_));
			serverFromClientSession_->onStanzaReceived.connect(boost::bind(&Server::handleStanzaReceived, this, _1, serverFromClientSession_));
			serverFromClientSession_->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, serverFromClientSession_));
			serverFromClientSession_->start();
		}

		void handleNewLinkLocalConnection(boost::shared_ptr<Connection>) {
			std::cout << "Incoming link local connection" << std::endl;
		}
		
		void handleServiceRegistered(const DNSSDService::Service& service) {
			std::cout << "Service registered " << service.name << " " << service.type << " " << service.domain << std::endl;
		}

		void handleSessionFinished(boost::shared_ptr<ServerFromClientSession>) {
			serverFromClientSession_.reset();
			unregisterService();
			rosterRequested_ = false;
		}

		void unregisterService() {
			if (dnsSDServiceRegistered_) {
				dnsSDServiceRegistered_ = false;
				dnsSDService_->unregisterService();
			}
		}

		void handleStanzaReceived(boost::shared_ptr<Stanza> stanza, boost::shared_ptr<ServerFromClientSession> session) {
			stanza->setFrom(session->getJID());
			if (!stanza->getTo().isValid()) {
				stanza->setTo(JID(session->getDomain()));
			}

			if (boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(stanza)) {
				if (presence->getType() == Presence::Available) {
					if (!dnsSDServiceRegistered_) {
						dnsSDServiceRegistered_ = true;
						dnsSDService_->registerService(session->getJID().toBare().toString(), linkLocalConnectionPort_, getLinkLocalServiceInfo(presence));
					}
					else {
						dnsSDService_->updateService(getLinkLocalServiceInfo(presence));
					}
				}
				else {
					unregisterService();
				}
			}
			else if (!stanza->getTo().isValid() || stanza->getTo() == session->getDomain() || stanza->getTo() == session->getJID().toBare()) {
				if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza)) {
					if (iq->getPayload<RosterPayload>()) {
						if (iq->getType() == IQ::Get) {
							session->sendStanza(IQ::createResult(iq->getFrom(), iq->getID(), linkLocalRoster_->getRoster()));
							rosterRequested_ = true;
							foreach(const boost::shared_ptr<Presence> presence, linkLocalRoster_->getAllPresence()) {
								session->sendStanza(presence);
							}
						}
						else {
							session->sendStanza(IQ::createError(iq->getFrom(), iq->getID(), Error::Forbidden, Error::Cancel));
						}
					}
					if (iq->getPayload<VCard>()) {
						if (iq->getType() == IQ::Get) {
							boost::shared_ptr<VCard> vcard(new VCard());
							vcard->setNickname(iq->getFrom().getNode());
							session->sendStanza(IQ::createResult(iq->getFrom(), iq->getID(), vcard));
						}
						else {
							session->sendStanza(IQ::createError(iq->getFrom(), iq->getID(), Error::Forbidden, Error::Cancel));
						}
					}
					else {
						session->sendStanza(IQ::createError(iq->getFrom(), iq->getID(), Error::FeatureNotImplemented, Error::Cancel));
					}
				}
			}
		}

		void handleRosterChanged(boost::shared_ptr<RosterPayload> roster) {
			if (rosterRequested_) {
				boost::shared_ptr<IQ> iq = IQ::createRequest(IQ::Set, serverFromClientSession_->getJID(), idGenerator_.generateID(), roster);
				iq->setFrom(serverFromClientSession_->getJID().toBare());
				serverFromClientSession_->sendStanza(iq);
			}
		}

		void handlePresenceChanged(boost::shared_ptr<Presence> presence) {
			if (rosterRequested_) {
				serverFromClientSession_->sendStanza(presence);
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
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
		bool dnsSDServiceRegistered_;
		bool rosterRequested_;
		int clientConnectionPort_;
		int linkLocalConnectionPort_;
};

int main() {
	SimpleEventLoop eventLoop;
	Server server(5222, 5562);
	eventLoop.run();
	return 0;
}
