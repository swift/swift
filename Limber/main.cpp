#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Server/SimpleUserRegistry.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Elements/Stanza.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class BoostConnectionServer : public ConnectionServer, public EventOwner, public boost::enable_shared_from_this<BoostConnectionServer> {
	public:
		BoostConnectionServer(int port, boost::asio::io_service& ioService) : acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
		}

		void start() {
			acceptNextConnection();
		}

	private:
		void acceptNextConnection() {
			boost::shared_ptr<BoostConnection> newConnection(new BoostConnection(&acceptor_.io_service()));
			acceptor_.async_accept(newConnection->getSocket(), 
				boost::bind(&BoostConnectionServer::handleAccept, shared_from_this(), newConnection, boost::asio::placeholders::error));
		}

		void handleAccept(boost::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error) {
			if (!error) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onNewConnection), newConnection), shared_from_this());
				newConnection->listen();
				acceptNextConnection();
			}
		}

		boost::asio::ip::tcp::acceptor acceptor_;
};

class Server {
	public:
		Server(UserRegistry* userRegistry) : userRegistry_(userRegistry) {
			serverFromClientConnectionServer_ = boost::shared_ptr<BoostConnectionServer>(new BoostConnectionServer(5224, boostIOServiceThread_.getIOService()));
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
