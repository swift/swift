/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

#include <boost/bind.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/MIX/MIX.h>
#include <Swiften/Network/BoostConnection.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/ConnectionServer.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

#include <Limber/Server/ServerFromClientSession.h>
#include <Limber/Server/SimpleUserRegistry.h>

#include <Swiften/Base/Log.h>

using namespace Swift;

class SimpleMIXChannelRegistry {

    public:
        SimpleMIXChannelRegistry() {}

        void addMIXChannel(const JID& channelJID) {
            channels_.insert(channelJID);
        }

        const std::unordered_set<std::string>& getChannels() {
            return channels_;
        }

        bool hasMIXChannel(const JID& jid) {
            return channels_.find(jid) != channels_.end();
        }

    private:
        std::unordered_set<std::string> channels_;
};

class Server {
    public:
        Server(UserRegistry* userRegistry, SimpleMIXChannelRegistry* mixChannelRegistry, EventLoop* eventLoop) : userRegistry_(userRegistry), mixChannelRegistry_(mixChannelRegistry) {
            serverFromClientConnectionServer_ = BoostConnectionServer::create(5222, boostIOServiceThread_.getIOService(), eventLoop);
            serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
            serverFromClientConnectionServer_->start();
        }

    private:
        void handleNewConnection(std::shared_ptr<Connection> c) {
            auto session  = std::make_shared<ServerFromClientSession>(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, &xmlParserFactory_, userRegistry_);
            serverFromClientSessions_.push_back(session);
            session->onElementReceived.connect(boost::bind(&Server::handleElementReceived, this, _1, session));
            session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
            session->startSession();
        }

        void handleSessionFinished(std::shared_ptr<ServerFromClientSession> session) {
            serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
        }

        void handleElementReceived(std::shared_ptr<ToplevelElement> element, std::shared_ptr<ServerFromClientSession> session) {
            std::shared_ptr<Stanza> stanza(std::dynamic_pointer_cast<Stanza>(element));
            if (!stanza) {
                return;
            }
            stanza->setFrom(session->getRemoteJID());

            auto iq = std::dynamic_pointer_cast<IQ>(stanza);
            if (!iq) {
                return;
            }

            if (mixChannelRegistry_->hasMIXChannel(stanza->getTo())) {
                //If request comes to particular channel supported by service.
                handleElementReceivedForMIXChannel(iq, session);
            } else if (stanza->getTo() == session->getLocalJID()) {
                //If request comes to domain service
                handleElementReceivedForMIXService(iq, session);
            } else if (stanza->getTo() == session->getRemoteJID().toBare() || stanza->getTo() == JID()) {
                //If request comes to own local server
                handleElementReceivedForAccount(iq, session);
            }
        }

        void handleElementReceivedForAccount(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session) {
            if(iq->getPayload<RosterPayload>()) {
                handleRosterRequest(iq, session);
            } else if (auto incomingJoinPayload = iq->getPayload<MIXJoin>()) {
                handleJoin(iq, session, incomingJoinPayload);
            } else if (auto incomingLeavePayload = iq->getPayload<MIXLeave>()) {
                handleLeave(iq, session, incomingLeavePayload);
            }
        }

        void handleElementReceivedForMIXService(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session) {
            if (iq->getPayload<DiscoItems>()) {
                SWIFT_LOG(debug) << "Query: Channel List" << std::endl;
                auto responsePayload = std::make_shared<DiscoItems>();
                for (auto channel : mixChannelRegistry_->getChannels()) {
                    responsePayload->addItem(DiscoItems::Item(JID(channel).getNode(), JID(channel)));
                }
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));
            }
        }

        void handleElementReceivedForMIXChannel(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session) {
            if (iq->getPayload<DiscoItems>()->getNode() == "mix") {
                SWIFT_LOG(debug) << "Query: Channel Nodes Supported" << std::endl;
                auto responsePayload = std::make_shared<DiscoItems>();
                responsePayload->addItem(DiscoItems::Item(MIX::ParticipantsNode, JID(iq->getTo())));
                responsePayload->addItem(DiscoItems::Item(MIX::MessagesNode, JID(iq->getTo())));
                responsePayload->addItem(DiscoItems::Item(MIX::PresenceNode, JID(iq->getTo())));
                responsePayload->addItem(DiscoItems::Item(MIX::JIDMapNode, JID(iq->getTo())));
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));
            } else {
                session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::BadRequest, ErrorPayload::Cancel));
            }
        }

        void handleRosterRequest(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session) {
            SWIFT_LOG(debug) << "Query: Roster" << std::endl;
            auto i = rosterMap_.find(iq->getFrom().toBare());
            if (i != rosterMap_.end()) {
                SWIFT_LOG(debug) << "Query: Roster Found." << std::endl;
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), getRosterPayloadFromXMPPRoster(i->second)));
            } else {
                SWIFT_LOG(debug) << "Query: Roster Not Found. Creating New." << std::endl;
                auto newRoster = std::make_shared<XMPPRosterImpl>();
                rosterMap_.insert(std::make_pair(iq->getFrom().toBare(), newRoster));
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), getRosterPayloadFromXMPPRoster(newRoster)));
            }
        }

        void handleJoin(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session, MIXJoin::ref incomingJoinPayload) {
            if (auto channelJID = incomingJoinPayload->getChannel()) {
                SWIFT_LOG(debug) << "Request: Channel Join - " << *channelJID << std::endl;
                if (mixChannelRegistry_->hasMIXChannel(*channelJID)) {

                    auto responsePayload = createJoinResult(incomingJoinPayload->getSubscriptions());
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));

                    auto i = rosterMap_.find(iq->getFrom().toBare());
                    if (i != rosterMap_.end()) {
                        auto roster = i->second;
                        roster->addContact(*channelJID, channelJID->getNode(), std::vector<std::string>(), RosterItemPayload::Subscription::Both, true);

                        auto rosterUpdatePayload = std::make_shared<RosterPayload>();
                        auto itemPayload = RosterItemPayload(*channelJID, channelJID->getNode(), RosterItemPayload::Subscription::Both);
                        itemPayload.setMIXChannel(true);
                        rosterUpdatePayload->addItem(itemPayload);
                        session->sendElement(IQ::createRequest(IQ::Set, iq->getFrom(), iq->getID(), rosterUpdatePayload));

                    } else {
                        SWIFT_LOG(debug) << "Initial roster not requested by client." <<std::endl;
                    }
                } else {
                    SWIFT_LOG(debug) << "Channel " << *channelJID << "not hosted on domain." <<std::endl;
                }
            } else {
                session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::BadRequest, ErrorPayload::Cancel));
            }
        }

        void handleLeave(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session, MIXLeave::ref incomingLeavePayload) {
            if (auto channelJID = incomingLeavePayload->getChannel()) {
                SWIFT_LOG(debug) << "Request: Channel Leave - " << *channelJID << std::endl;
                if (mixChannelRegistry_->hasMIXChannel(*channelJID)) {

                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), std::make_shared<MIXLeave>()));

                    auto i = rosterMap_.find(iq->getFrom().toBare());
                    if (i != rosterMap_.end()) {
                        auto roster = i->second;
                        roster->removeContact(*channelJID);

                        auto rosterUpdatePayload = std::make_shared<RosterPayload>();
                        auto itemPayload = RosterItemPayload(*channelJID, channelJID->getNode(), RosterItemPayload::Subscription::Remove);
                        itemPayload.setMIXChannel(true);
                        rosterUpdatePayload->addItem(itemPayload);

                        session->sendElement(IQ::createRequest(IQ::Set, iq->getFrom(), iq->getID(), rosterUpdatePayload));
                    } else {
                        SWIFT_LOG(debug) << "Initial roster not requested by client." <<std::endl;
                    }
                } else {
                    SWIFT_LOG(debug) << "Channel " << *channelJID << "not hosted on domain." <<std::endl;
                }
            } else {
                session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::BadRequest, ErrorPayload::Cancel));
            }
        }

        MIXJoin::ref createJoinResult(const std::unordered_set<std::string>& nodes) {
            auto joinResultPayload = std::make_shared<MIXJoin>();
            for (auto node : nodes) {
                joinResultPayload->addSubscription(node);
            }
            joinResultPayload->setJID(JID("123456#coven@example.com"));
            return joinResultPayload;
        }

        std::shared_ptr<RosterPayload> getRosterPayloadFromXMPPRoster(std::shared_ptr<XMPPRosterImpl> roster) {
            auto rosterPayload = std::make_shared<RosterPayload>();
            for (auto item : roster->getItems()) {
                auto itemPayload = RosterItemPayload(item.getJID(), item.getName(), item.getSubscription());
                if (item.isMIXChannel()) {
                    itemPayload.setMIXChannel(true);
                }
                rosterPayload->addItem(itemPayload);
            }
            return rosterPayload;
        }

    private:
        IDGenerator idGenerator_;
        PlatformXMLParserFactory xmlParserFactory_;
        UserRegistry* userRegistry_;
        SimpleMIXChannelRegistry* mixChannelRegistry_;
        BoostIOServiceThread boostIOServiceThread_;
        std::shared_ptr<BoostConnectionServer> serverFromClientConnectionServer_;
        std::vector< std::shared_ptr<ServerFromClientSession> > serverFromClientSessions_;
        FullPayloadParserFactoryCollection payloadParserFactories_;
        FullPayloadSerializerCollection payloadSerializers_;

        using RosterMap = std::map<JID, std::shared_ptr<XMPPRosterImpl>>;
        RosterMap rosterMap_;
};

int main() {
    //Log::setLogLevel(Log::Severity::debug);

    SimpleEventLoop eventLoop;
    SimpleUserRegistry userRegistry;

    userRegistry.addUser(JID("some@example.com"), "mixtest");
    userRegistry.addUser(JID("another@example.com"), "mixtest2");

    SimpleMIXChannelRegistry mixChannelRegistry;
    mixChannelRegistry.addMIXChannel(JID("coven@example.com"));

    Server server(&userRegistry, &mixChannelRegistry, &eventLoop);
    eventLoop.run();
    return 0;
}
