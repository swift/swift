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
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXSetNick.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubItems.h>
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

        const boost::optional<std::string>& getNick() {
            return nick_;
        }

        void setProxyJID(JID proxyJID) {
            proxyJID_ = proxyJID;
        }

        void setRoster(std::shared_ptr<XMPPRosterImpl> roster) {
            roster_ = roster;
        }

        void setNick(std::string nick) {
            nick_ = nick;
        }

    private:
        boost::optional<std::string> nick_;
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

        void handlePresenceReceived(Presence::ref presence) {
            SWIFT_LOG(debug) << "Presence Received" << std::endl;
            auto channelJID = presence->getTo();
            auto sender = presence->getFrom();
            auto j = presenceMap_.find(sender);
            if (j != presenceMap_.end()) {
                SWIFT_LOG(debug) << "Updating Presence" << std::endl;
                auto currentPresenceStatus = j->second;
                j->second = presence->getType();

                //Coming online after being online. Returning presence of channel to client.
                if (currentPresenceStatus == Presence::Type::Unavailable && presence->getType() == Presence::Type::Available) {
                    SWIFT_LOG(debug) << "Client coming online after being online " << std::endl;
                    sendChannelPresence(channelJID, sender);
                }
            } else {
                SWIFT_LOG(debug) << "Presence information not found for " << sender << std::endl;
            }

            SWIFT_LOG_ASSERT(mixChannelRegistry_->hasMIXChannel(channelJID), warning);
            sendPresenceStatusToClients(sender, channelJID, presence);
        }

        void sendChannelPresence(const JID& channelJID, const JID& recipient) {
            SWIFT_LOG(debug) << "Sending channel presence to " << recipient << std::endl;
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                auto participants = i->second;
                auto forwardPresence = Presence::create();
                forwardPresence->setID(idGenerator_.generateID());
                for (auto participant : participants) {
                    // Not sending self presence information.
                    if (participant != recipient.toString()) {
                        SWIFT_LOG(debug) << "Participant: " << participant << std::endl;
                        auto k = participantInformationMap_.find(JID(participant).toBare());
                        if (k != participantInformationMap_.end()) {
                            if ((k->second)->getProxyJID()) {
                                forwardPresence->setFrom(*((k->second)->getProxyJID()));
                            }
                        }

                        auto j = presenceMap_.find(JID(participant));
                        if (j != presenceMap_.end()) {
                            forwardPresence->setType(j->second);
                        }

                        auto t = sessionMap_.find(recipient);
                        if (t != sessionMap_.end()) {
                            forwardPresence->setTo(recipient);
                            (t->second)->sendElement(forwardPresence);
                        }
                    }
                }
            }
        }

        void sendPresenceStatusToClients(const JID& sender, const JID& channelJID, Presence::ref newPresence) {
            auto i = participantMap_.find(channelJID);
            if (i != participantMap_.end()) {
                auto participants = i->second;

                auto forwardPresence = Presence::create();
                forwardPresence->setType(newPresence->getType());
                forwardPresence->setID(idGenerator_.generateID());

                if (information_->getProxyJID()) {
                    SWIFT_LOG_ASSERT(information_->getProxyJID(), warning);
                    forwardPresence->setFrom(*information_->getProxyJID());
                }

                for (auto participant : participants) {
                    if (participant != sender.toString()) {
                        auto t = presenceMap_.find(JID(participant));
                        if (t != presenceMap_.end()) {
                            if (t->second == Presence::Type::Unavailable) {
                                SWIFT_LOG(debug) << "Client" << participant << "not Available" << std::endl;
                                continue;
                            }
                        } else {
                            SWIFT_LOG(debug) << "Presence information not found for " << participant << std::endl;
                        }

                        SWIFT_LOG(debug) << "Forwarding Presence to " << participant << std::endl;
                        forwardPresence->setTo(JID(participant));
                        auto k = sessionMap_.find(JID(participant));
                        if (k != sessionMap_.end()) {
                            SWIFT_LOG(debug) << "Session Found for " << participant << std::endl;
                            (k->second)->sendElement(forwardPresence);
                        }
                    }
                }
            }
        }

        void handleMessageReceived(Message::ref message, std::shared_ptr<ServerFromClientSession> session) {
            SWIFT_LOG(debug) << "Message Received" << std::endl;
            auto channelJID = message->getTo();
            auto sender = message->getFrom();
            SWIFT_LOG(debug) << "Channel " << channelJID << std::endl;
            SWIFT_LOG(debug) << "Sender " << sender << std::endl;

            SWIFT_LOG_ASSERT(mixChannelRegistry_->hasMIXChannel(channelJID), warning);
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
                    SWIFT_LOG_ASSERT(information_->getProxyJID(), warning);
                    mixPayload->setJID(*information_->getProxyJID());
                }

                if (information_->getNick()) {
                    SWIFT_LOG_ASSERT(information_->getNick(), warning);
                    mixPayload->setNick(*information_->getNick());
                }

                forwardMessage->addPayload(mixPayload);

                for (auto participant : participants) {
                    if (participant != sender.toString()) {
                        auto t = presenceMap_.find(JID(participant));
                        if (t != presenceMap_.end()) {
                            if (t->second == Presence::Type::Unavailable) {
                                SWIFT_LOG(debug) << "Client" << participant << "not Available" << std::endl;
                                continue;
                            }
                        } else {
                            SWIFT_LOG(debug) << "Presence information not found for " << participant << std::endl;
                        }

                        SWIFT_LOG(debug) << "Forwarding Message to " << participant << std::endl;
                        forwardMessage->setTo(JID(participant));
                        auto k = sessionMap_.find(JID(participant));
                        if (k != sessionMap_.end()) {
                            SWIFT_LOG(debug) << "Session Found for " << participant << std::endl;
                            (k->second)->sendElement(forwardMessage);
                        }
                    }
                }

                mixPayload->setSubmissionID(message->getID());
                forwardMessage->setTo(sender);
                session->sendElement(forwardMessage);
            }
        }

        void handleElementReceivedForAccount(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            auto iq = std::dynamic_pointer_cast<IQ>(stanza);
            if (!iq) {
                return;
            }

            if(iq->getPayload<RosterPayload>()) {
                handleRosterRequest(iq, session);
            } else if (auto incomingJoinPayload = iq->getPayload<MIXJoin>()) {
                handleJoin(iq, session, incomingJoinPayload);
            } else if (auto incomingLeavePayload = iq->getPayload<MIXLeave>()) {
                handleLeave(iq, session, incomingLeavePayload);
            }
        }

        void handleElementReceivedForMIXService(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            auto iq = std::dynamic_pointer_cast<IQ>(stanza);
            if (!iq) {
                return;
            }

            if (iq->getPayload<DiscoItems>()) {
                SWIFT_LOG(debug) << "Query: Channel List" << std::endl;
                auto responsePayload = std::make_shared<DiscoItems>();
                for (auto channel : mixChannelRegistry_->getChannels()) {
                    responsePayload->addItem(DiscoItems::Item(JID(channel).getNode(), JID(channel)));
                }
                session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), responsePayload));
            }
        }

        void handleElementReceivedForMIXChannel(Stanza::ref stanza, std::shared_ptr<ServerFromClientSession> session) {
            auto message = std::dynamic_pointer_cast<Message>(stanza);
            if (message) {
                handleMessageReceived(message, session);
                return;
            }

            auto presence = std::dynamic_pointer_cast<Presence>(stanza);
            if (presence) {
                handlePresenceReceived(presence);
                return;
            }

            auto iq = std::dynamic_pointer_cast<IQ>(stanza);
            if (!iq) {
                return;
            }

            if (auto setNickPayload = iq->getPayload<MIXSetNick>()) {
                SWIFT_LOG(debug) << "Query: Set Nick" << std::endl;
                handleSetNickRequest(iq, session, setNickPayload);
            } else if (auto pubSubPayload = iq->getPayload<PubSub>()) {
                SWIFT_LOG(debug) << "Query: PubSub" << std::endl;
                auto channelJID = iq->getTo();
                SWIFT_LOG_ASSERT(mixChannelRegistry_->hasMIXChannel(channelJID), warning);
                auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(pubSubPayload->getPayload());
                if (itemsPayload->getNode() == MIX::ParticipantsNode) {
                    SWIFT_LOG(debug) << "Query: Participant List for " << channelJID << std::endl;
                    session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), getParticipants(channelJID)));
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

        void handleSetNickRequest(IQ::ref iq, std::shared_ptr<ServerFromClientSession> session, MIXSetNick::ref payload) {
            auto requestedNick = payload->getNick();
            for ( auto iter = participantInformationMap_.begin(), iend = participantInformationMap_.end(); iter != iend; ++iter ) {
                if ((iter->second)->getNick() && *((iter->second)->getNick()) == requestedNick) {
                    SWIFT_LOG(debug) << "Nick Conflict" << requestedNick << std::endl;
                    session->sendElement(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::Conflict, ErrorPayload::Cancel));
                    return;
                }
            }
            information_->setNick(requestedNick);
            auto setNickPayload = std::make_shared<MIXSetNick>();
            setNickPayload->setNick(requestedNick);
            session->sendElement(IQ::createResult(iq->getFrom(), iq->getTo(), iq->getID(), setNickPayload));
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
                        presenceMap_.insert(std::make_pair(iq->getFrom(),Presence::Type::Available));

                        auto j = sessionMap_.find(iq->getFrom());
                        if (j == sessionMap_.end()) {
                            SWIFT_LOG(debug) << "Inserting " << iq->getFrom() << " with " << &session << std::endl;
                            sessionMap_.insert(std::make_pair(iq->getFrom(), session));
                        }

                        //returning presence information for channel.
                        sendChannelPresence(*channelJID, iq->getFrom());

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
                        presenceMap_.erase(iq->getFrom());

                        auto j = sessionMap_.find(iq->getFrom());
                        if (j != sessionMap_.end()) {
                            sessionMap_.erase(iq->getFrom());
                        }

                        auto k = participantInformationMap_.find(iq->getFrom().toBare());
                        if (k != participantInformationMap_.end()) {
                            participantInformationMap_.erase(iq->getFrom().toBare());
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

        std::shared_ptr<PubSub> getParticipants(const JID& channelJID) {
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

                        if ((j->second)->getNick()) {
                            mixParticipant->setNick(*((j->second)->getNick()));
                        } else {
                            SWIFT_LOG(debug) << "Participant" << JID(participant).toBare() << "not found in NickMap." << std::endl;
                        }
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

        // Maps client JID to its presence.
        using PresenceMap = std::map<JID, Presence::Type>;
        PresenceMap presenceMap_;

        // Maps user JID to its information object.
        using MIXParticipantInformationMap = std::map<JID, std::shared_ptr<MIXParticipantInformation>>;
        MIXParticipantInformationMap participantInformationMap_;
};

int main() {
    Log::setLogLevel(Log::Severity::debug);

    SimpleEventLoop eventLoop;
    SimpleUserRegistry userRegistry;

    userRegistry.addUser(JID("some@example.com"), "mixtest");
    userRegistry.addUser(JID("another@example.com"), "mixtest");
    userRegistry.addUser(JID("yetanother@example.com"), "mixtest");

    SimpleMIXChannelRegistry mixChannelRegistry;
    mixChannelRegistry.addMIXChannel(JID("coven@example.com"));

    Server server(&userRegistry, &mixChannelRegistry, &eventLoop);
    eventLoop.run();
    return 0;
}
