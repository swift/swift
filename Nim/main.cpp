#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"
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
		Server(int clientConnectionPort, int linkLocalConnectionPort) : dnsSDServiceRegistered_(false), clientConnectionPort_(clientConnectionPort), linkLocalConnectionPort_(linkLocalConnectionPort) {
			serverFromClientConnectionServer_ = boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(clientConnectionPort, &boostIOServiceThread_.getIOService()));
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
			serverFromClientConnectionServer_->start();

			dnsSDService_ = boost::shared_ptr<AppleDNSSDService>(new AppleDNSSDService());
			linkLocalRoster_ = boost::shared_ptr<LinkLocalRoster>(new LinkLocalRoster(dnsSDService_));
			dnsSDService_->start();
		}

	private:
		void handleNewConnection(boost::shared_ptr<Connection> c) {
			boost::shared_ptr<ServerFromClientSession> session(new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &userRegistry_));
			serverFromClientSessions_.push_back(session);
			session->onStanzaReceived.connect(boost::bind(&Server::handleStanzaReceived, this, _1, session));
			session->onSessionStarted.connect(boost::bind(&Server::handleSessionStarted, this, session));
			session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
			session->start();
		}
		
		void handleSessionStarted(boost::shared_ptr<ServerFromClientSession> session) {
			if (!dnsSDServiceRegistered_) {
				dnsSDServiceRegistered_ = true;
				dnsSDService_->onServiceRegistered.connect(boost::bind(&Server::handleServiceRegistered, this, _1));
				LinkLocalServiceInfo info;
				info.setFirstName("Remko");
				info.setLastName("Tron\xc3\xe7on");
				info.setEMail("email@example.com");
				info.setJID(JID("jid@example.com"));
				info.setMessage("I'm not Here");
				info.setNick("remko");
				info.setStatus(LinkLocalServiceInfo::Away);
				info.setPort(linkLocalConnectionPort_);
				dnsSDService_->registerService(session->getJID().toBare().toString(), linkLocalConnectionPort_, info);
			}
		}

		void handleServiceRegistered(const DNSSDService::Service& service) {
			std::cout << "Service registered " << service.name << " " << service.type << " " << service.domain << std::endl;
		}

		void handleSessionFinished(boost::shared_ptr<ServerFromClientSession> session) {
			serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
			if (serverFromClientSessions_.empty()) {
				dnsSDServiceRegistered_ = false;
				dnsSDService_->unregisterService();
			}
		}

		void handleStanzaReceived(boost::shared_ptr<Stanza> stanza, boost::shared_ptr<ServerFromClientSession> session) {
			stanza->setFrom(session->getJID());
			if (!stanza->getTo().isValid()) {
				stanza->setTo(JID(session->getDomain()));
			}
			if (!stanza->getTo().isValid() || stanza->getTo() == session->getDomain() || stanza->getTo() == session->getJID().toBare()) {
				if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza)) {
					if (iq->getPayload<RosterPayload>()) {
						session->sendStanza(IQ::createResult(iq->getFrom(), iq->getID(), boost::shared_ptr<RosterPayload>(new RosterPayload())));
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

	private:
		IDGenerator idGenerator_;
		BoostIOServiceThread boostIOServiceThread_;
		DummyUserRegistry userRegistry_;
		boost::shared_ptr<AppleDNSSDService> dnsSDService_;
		boost::shared_ptr<LinkLocalRoster> linkLocalRoster_;
		boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
		std::vector< boost::shared_ptr<ServerFromClientSession> > serverFromClientSessions_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
		bool dnsSDServiceRegistered_;
		int clientConnectionPort_;
		int linkLocalConnectionPort_;
};

int main() {
	SimpleEventLoop eventLoop;
	Server server(5222, 5562);
	eventLoop.run();
	return 0;
}
