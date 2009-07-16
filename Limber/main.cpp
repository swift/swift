#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Server/SimpleUserRegistry.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Elements/Stanza.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class Server {
	public:
		Server(UserRegistry* userRegistry) : userRegistry_(userRegistry) {
			serverFromClientConnectionServer_ = boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(5224, &boostIOServiceThread_.getIOService()));
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
			serverFromClientConnectionServer_->start();
		}

	private:
		void handleNewConnection(boost::shared_ptr<Connection> c) {
			ServerFromClientSession* session = new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, userRegistry_);
			serverFromClientSessions_.push_back(session);
			session->onStanzaReceived.connect(boost::bind(&Server::handleStanzaReceived, this, _1, session));
			session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
		}

		void handleSessionFinished(ServerFromClientSession* session) {
			serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
			delete session;
		}

		void handleStanzaReceived(boost::shared_ptr<Stanza> stanza, ServerFromClientSession* session) {
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
		UserRegistry* userRegistry_;
		BoostIOServiceThread boostIOServiceThread_;
		boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
		std::vector<ServerFromClientSession*> serverFromClientSessions_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
};

int main() {
	SimpleEventLoop eventLoop;
	SimpleUserRegistry userRegistry;
	userRegistry.addUser(JID("remko@localhost"), "remko");
	userRegistry.addUser(JID("kevin@localhost"), "kevin");
	userRegistry.addUser(JID("remko@limber.swift.im"), "remko");
	userRegistry.addUser(JID("kevin@limber.swift.im"), "kevin");
	Server server(&userRegistry);
	eventLoop.run();
	return 0;
}
