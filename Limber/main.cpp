/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/RosterPayload.h"
#include "Swiften/Elements/VCard.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/EventLoop/EventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/EventLoop/EventOwner.h"
#include "Swiften/Elements/Stanza.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Network/BoostConnectionServer.h"
#include "Limber/Server/SimpleUserRegistry.h"
#include "Limber/Server/ServerFromClientSession.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Parser/PlatformXMLParserFactory.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class Server {
	public:
		Server(UserRegistry* userRegistry, EventLoop* eventLoop) : userRegistry_(userRegistry) {
			serverFromClientConnectionServer_ = BoostConnectionServer::create(5222, boostIOServiceThread_.getIOService(), eventLoop);
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
			serverFromClientConnectionServer_->start();
		}

	private:
		void handleNewConnection(boost::shared_ptr<Connection> c) {
			boost::shared_ptr<ServerFromClientSession> session(new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &xmlParserFactory, userRegistry_));
			serverFromClientSessions_.push_back(session);
			session->onElementReceived.connect(boost::bind(&Server::handleElementReceived, this, _1, session));
			session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
			session->startSession();
		}

		void handleSessionFinished(boost::shared_ptr<ServerFromClientSession> session) {
			serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
		}

		void handleElementReceived(boost::shared_ptr<Element> element, boost::shared_ptr<ServerFromClientSession> session) {
			boost::shared_ptr<Stanza> stanza(boost::dynamic_pointer_cast<Stanza>(element));
			if (!stanza) {
				return;
			}
			stanza->setFrom(session->getRemoteJID());
			if (!stanza->getTo().isValid()) {
				stanza->setTo(JID(session->getLocalJID()));
			}
			if (!stanza->getTo().isValid() || stanza->getTo() == session->getLocalJID() || stanza->getTo() == session->getRemoteJID().toBare()) {
				if (boost::shared_ptr<IQ> iq = boost::dynamic_pointer_cast<IQ>(stanza)) {
					if (iq->getPayload<RosterPayload>()) {
						session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), boost::make_shared<RosterPayload>()));
					}
					if (iq->getPayload<VCard>()) {
						if (iq->getType() == IQ::Get) {
							boost::shared_ptr<VCard> vcard(new VCard());
							vcard->setNickname(iq->getFrom().getNode());
							session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), vcard));
						}
						else {
							session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::Forbidden, ErrorPayload::Cancel));
						}
					}
					else {
						session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::FeatureNotImplemented, ErrorPayload::Cancel));
					}
				}
			}
		}

	private:
		IDGenerator idGenerator_;
		PlatformXMLParserFactory xmlParserFactory;
		UserRegistry* userRegistry_;
		BoostIOServiceThread boostIOServiceThread_;
		boost::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
		std::vector< boost::shared_ptr<ServerFromClientSession> > serverFromClientSessions_;
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
	Server server(&userRegistry, &eventLoop);
	eventLoop.run();
	return 0;
}
