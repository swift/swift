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
#include <Swiften/Base/Log.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/MIX/MIX.h>
#include <Swiften/Network/BoostConnection.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/ConnectionServer.h>
#include <Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

#include <Limber/Server/ServerFromClientSession.h>
#include <Limber/Server/SimpleUserRegistry.h>

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

class MIXParticipantInformation {

    public:
        using ref = std::shared_ptr<MIXParticipantInformation>;

    public:
        MIXParticipantInformation() {}

        const boost::optional<std::shared_ptr<XMPPRosterImpl>>& getRoster() {
            return roster_;
        }

        const boost::optional<JID>& getProxyJID() {
            return proxyJID_;
        }

        void setProxyJID(JID proxyJID) {
            proxyJID_ = proxyJID;
        }

        void setRoster(std::shared_ptr<XMPPRosterImpl> roster) {
            roster_ = roster;
        }

    private:
        boost::optional<JID> proxyJID_;
        boost::optional<std::shared_ptr<XMPPRosterImpl>> roster_;
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

            auto i = participantInformationMap_.find(session->getRemoteJID().toBare());
            if (i != participantInformationMap_.end()) {
                information_ = i->second;
            } else {
                information_ = std::make_shared<MIXParticipantInformation>();
                participantInformationMap_.insert(std::make_pair(session->getRemoteJID().toBare(), information_));
            }

            if (mixChannelRegistry_->hasMIXChannel(stanza->getTo())) {
                //If request comes to particular channel supported by service.
                handleElementReceivedForMIXChannel(stanza, session);
            } else if (stanza->getTo() == session->getLocalJID()) {
                //If request comes to domain service.
                handleElementReceivedForMIXService(stanza, session);
            } else if (stanza->getTo() == session->getRemoteJID().toBare() || stanza->getTo() == JID()) {
                //If request comes to own account.
                handleElementReceivedForAccount(stanza, session);
            }
        }

        /**
        *  If message is received by MIX channel, it sends it to all clients with JID as proxy JID of sender.
        *  It sends a replicated message with same submission ID as received message to the sender for verification.
        */
        void handleMessageReceivedForMIXChannel(Message::ref message, std::shared_ptr<ServerFromClientSession> session) {
            SWIFT_LOG(debug) << "Message Received" << std::endl;
            auto channelJID = message->getTo();
            auto sender = message->getFrom();
            SWIFT_LOG(debug) << "Channel " << channelJID << std::endl;
            SWIFT_LOG(debug) << "Sender " << sender << std::endl;

            SWIFT_LOG_ASSERT(mixChannelRegistry_->hasMIXChannel(channelJID), warning);

            // Send message to all clients.
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                auto participants = i->second;

                auto forwardMessage = std::make_shared<Message>();
                forwardMessage->setFrom(channelJID);
                forwardMessage->setType(Message::Groupchat);
                forwardMessage->setBody(message->getBody());
                forwardMessage->setID(idGenerator_.generateID());

                auto mixPayload = std::make_shared<MIXPayload>();

                if (information_->getProxyJID()) {
                    mixPayload->setJID(*information_->getProxyJID());
                }

                forwardMessage->addPayload(mixPayload);

                for (auto participant : participants) {
                    if (participant != sender.toString()) {
                        SWIFT_LOG(debug) << "Forwarding to " << participant << std::endl;
                        forwardMessage->setTo(JID(participant));
                        auto k = sessionMap_.find(JID(participant));
                        if (k != sessionMap_.end()) {
                            SWIFT_LOG(debug) << "Session Found for " << participant << std::endl;
                            (k->second)->sendElement(forwardMessage);
                        }
                    }
                }

                // Sending replicated message to sender with same submission ID as received message.
                mixPayload->setSubmissionID(message->getID());
                forwardMessage->setTo(sender);
                session->sendElement(forwardMessage);
            }
        }

        void handleElementReceivedForAccount(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            if (auto iq = std::dynamic_pointer_cast<IQ>(stanza)) {
                if(iq->getPayload<RosterPayload>()) {
                    handleRosterRequest(iq, session);
                } else if (auto incomingJoinPayload = iq->getPayload<MIXJoin>()) {
                    handleJoin(iq, session, incomingJoinPayload);
                } else if (auto incomingLeavePayload = iq->getPayload<MIXLeave>()) {
                    handleLeave(iq, session, incomingLeavePayload);
                }
            }
        }

        void handleElementReceivedForMIXService(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            if (auto iq = std::dynamic_pointer_cast<IQ>(stanza)) {
                if (iq->getPayload<DiscoItems>()) {
                    SWIFT_LOG(debug) << "Query: Channel List" << std::endl;
                    auto responsePayload = std::make_shared<DiscoItems>();
                    for (auto channel : mixChannelRegistry_->getChannels()) {
                        responsePayload->addItem(DiscoItems::Item(JID(channel).getNode(), JID(channel)));
                    }
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));
                }
            }
        }

        void handleElementReceivedForMIXChannel(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            if (auto message = std::dynamic_pointer_cast<Message>(stanza)) {
                handleMessageReceivedForMIXChannel(message, session);
            } else if (auto iq = std::dynamic_pointer_cast<IQ>(stanza)) {
                handleIQReceivedForMIXChannel(iq, session);
            }
        }

        void handleIQReceivedForMIXChannel(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session) {
            if (auto pubSubPayload = iq->getPayload<PubSub>()) {
                SWIFT_LOG(debug) << "Query: PubSub" << std::endl;
                auto channelJID = iq->getTo();
                SWIFT_LOG_ASSERT(mixChannelRegistry_->hasMIXChannel(channelJID), warning);
                auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(pubSubPayload->getPayload());
                if (itemsPayload->getNode() == MIX::ParticipantsNode) {
                    SWIFT_LOG(debug) << "Query: Participant List for " << channelJID << std::endl;
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), retrieveParticipantsOfChannel(channelJID)));
                } else if (itemsPayload->getNode() == MIX::JIDMapNode) {
                    SWIFT_LOG(debug) << "Query: Lookup Participants from " << channelJID << std::endl;
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), getRealJIDResponse(pubSubPayload)));
                }
            } else if (iq->getPayload<DiscoItems>()->getNode() == "mix") {
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
            if (information_->getRoster()) {
                SWIFT_LOG(debug) << "Query: Roster Found." << std::endl;
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), getRosterPayloadFromXMPPRoster(*information_->getRoster())));
            } else {
                SWIFT_LOG(debug) << "Query: Roster Not Found. Creating New." << std::endl;
                auto newRoster = std::make_shared<XMPPRosterImpl>();
                information_->setRoster(newRoster);
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getID(), getRosterPayloadFromXMPPRoster(newRoster)));
            }
        }

        void handleJoin(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session, MIXJoin::ref incomingJoinPayload) {
            if (auto channelJID = incomingJoinPayload->getChannel()) {
                SWIFT_LOG(debug) << "Request: Channel Join - " << *channelJID << std::endl;
                if (mixChannelRegistry_->hasMIXChannel(*channelJID)) {

                    auto responsePayload = createJoinResult(*channelJID, incomingJoinPayload->getSubscriptions());
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));

                    if (information_->getRoster()) {
                        auto roster = *information_->getRoster();
                        roster->addContact(*channelJID, channelJID->getNode(), std::vector<std::string>(), RosterItemPayload::Subscription::Both, true);

                        auto rosterUpdatePayload = std::make_shared<RosterPayload>();
                        auto itemPayload = RosterItemPayload(*channelJID, channelJID->getNode(), RosterItemPayload::Subscription::Both);
                        itemPayload.setMIXChannel(true);
                        rosterUpdatePayload->addItem(itemPayload);
                        session->sendElement(IQ::createRequest(IQ::Set, iq->getFrom(), iq->getID(), rosterUpdatePayload));

                        updateParticipantNode(*channelJID, iq->getFrom());

                        auto j = sessionMap_.find(iq->getFrom());
                        if (j == sessionMap_.end()) {
                            SWIFT_LOG(debug) << "Inserting " << iq->getFrom() << " with " << &session << std::endl;
                            sessionMap_.insert(std::make_pair(iq->getFrom(), session));
                        }

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

                    if (information_->getRoster()) {
                        auto roster = *information_->getRoster();
                        roster->removeContact(*channelJID);

                        auto rosterUpdatePayload = std::make_shared<RosterPayload>();
                        auto itemPayload = RosterItemPayload(*channelJID, channelJID->getNode(), RosterItemPayload::Subscription::Remove);
                        itemPayload.setMIXChannel(true);
                        rosterUpdatePayload->addItem(itemPayload);

                        session->sendElement(IQ::createRequest(IQ::Set, iq->getFrom(), iq->getID(), rosterUpdatePayload));

                        removeParticipantFromNode(*channelJID, iq->getFrom());

                        auto j = sessionMap_.find(iq->getFrom());
                        if (j != sessionMap_.end()) {
                            sessionMap_.erase(iq->getFrom());
                        }

                        auto k = participantInformationMap_.find(iq->getFrom());
                        if (k != participantInformationMap_.end()) {
                            participantInformationMap_.erase(iq->getFrom());
                        }
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

        void updateParticipantNode(const JID& channelJID, const JID& participantJID) {
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                (i->second).insert(participantJID);
            } else {
                std::unordered_set<std::string> participants;
                participants.insert(participantJID);
                participantMap_.insert(std::make_pair(channelJID, participants));
            }
        }

        void removeParticipantFromNode(const JID& channelJID, const JID& participantJID) {
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                (i->second).erase(participantJID);
            }
        }

        MIXJoin::ref createJoinResult(const JID& channelJID, const std::unordered_set<std::string>& nodes) {
            auto joinResultPayload = std::make_shared<MIXJoin>();
            for (auto node : nodes) {
                joinResultPayload->addSubscription(node);
            }
            auto proxyJID = idGenerator_.generateID();

            if (information_->getProxyJID()) {
                proxyJID = *information_->getProxyJID();
            } else {
                proxyJID += ("#"+channelJID.toString());
                information_->setProxyJID(proxyJID);
            }

            joinResultPayload->setJID(JID(proxyJID));

            return joinResultPayload;
        }

        static std::shared_ptr<RosterPayload> getRosterPayloadFromXMPPRoster(std::shared_ptr<XMPPRosterImpl> roster) {
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

        std::shared_ptr<PubSub> retrieveParticipantsOfChannel(const JID& channelJID) {
            auto pubSubPayload = std::make_shared<PubSub>();
            auto pubSubItems = std::make_shared<PubSubItems>();
            pubSubItems->setNode(MIX::ParticipantsNode);
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                auto participants = i->second;
                for (auto participant : participants) {

                    auto itemPayload = std::make_shared<PubSubItem>();
                    auto mixParticipant = std::make_shared<MIXParticipant>();

                    auto j = participantInformationMap_.find(JID(participant).toBare());
                    if (j != participantInformationMap_.end()) {
                        SWIFT_LOG_ASSERT((j->second)->getProxyJID(), warning);
                        itemPayload->setID(*((j->second)->getProxyJID()));
                    } else {
                        SWIFT_LOG(debug) << "Participant" << JID(participant).toBare() << "not found in JIDMap." << std::endl;
                    }
                    itemPayload->addData(mixParticipant);
                    pubSubItems->addItem(itemPayload);
                }
            } else {
                SWIFT_LOG(debug) << "Channel" << channelJID << "not found in ParticipantMap." << std::endl;
            }
            pubSubPayload->setPayload(pubSubItems);
            return pubSubPayload;
        }

        std::shared_ptr<PubSub> getRealJIDResponse(std::shared_ptr<PubSub> payload) {
            auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(payload->getPayload());
            for (auto item : itemsPayload->getItems()) {
                for( auto iter = participantInformationMap_.begin(), iend = participantInformationMap_.end(); iter != iend; ++iter ) {
                    if ((iter->second)->getProxyJID() && *((iter->second)->getProxyJID()) == item->getID()) {
                        auto mixParticipant = std::make_shared<MIXParticipant>();
                        mixParticipant->setJID(iter->first);
                        item->addData(mixParticipant);
                        break;
                    }
                }
            }
            return payload;
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

        //Holds information about the current user JID.
        MIXParticipantInformation::ref information_;

        // Maps channel JID to its participants.
        using ParticipantMap = std::map<JID, std::unordered_set<std::string>>;
        ParticipantMap participantMap_;

        // Maps client JID to its session.
        using SessionMap = std::map<JID, std::shared_ptr<ServerFromClientSession>>;
        SessionMap sessionMap_;

        // Maps user JID to its information object.
        using MIXParticipantInformationMap = std::map<JID, std::shared_ptr<MIXParticipantInformation>>;
        MIXParticipantInformationMap participantInformationMap_;
};

int main() {
    Log::setLogLevel(Log::Severity::debug);

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
