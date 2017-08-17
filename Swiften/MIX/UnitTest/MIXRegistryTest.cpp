/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <boost/bind.hpp>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/MIX/MIXRegistry.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class MIXRegistryTest : public ::testing::Test {

    public:
        void SetUp() {
            ownJID_ = JID("hag66@shakespeare.example/UUID-a1j/7533");
            channel_ = new DummyStanzaChannel();
            router_ = new IQRouter(channel_);
            xmppRoster_ = new XMPPRosterImpl();
            successfulJoins_ = 0;
        }

        MIXRegistry* getMIXRegistry() {
            mixRegistry_ = new MIXRegistry(ownJID_, router_, xmppRoster_);
            mixRegistry_->onChannelJoined.connect(boost::bind(&MIXRegistryTest::handleChannelJoined, this, _1));
            mixRegistry_->onChannelJoinFailed.connect(boost::bind(&MIXRegistryTest::handleJoinFailed, this, _1));
            mixRegistry_->onChannelLeft.connect(boost::bind(&MIXRegistryTest::handleChannelLeft, this, _1));
            return mixRegistry_;
        }

        void TearDown() {
           delete mixRegistry_;
           delete router_;
           delete channel_;
           delete xmppRoster_;
        }

        void handleChannelJoined(const JID& /*jid*/) {
            ++successfulJoins_;
        }

        void handleJoinFailed(ErrorPayload::ref error) {
            ASSERT_TRUE(error);
        }

        void handleChannelLeft(const JID& /*jid*/) {
            --successfulJoins_;
        }

        IQ::ref createError() {
            return IQ::createError(ownJID_, channel_->sentStanzas[0]->getTo(), channel_->sentStanzas[0]->getID());
        }

        IQ::ref createJoinResult(const std::unordered_set<std::string>& nodes) {
            auto joinResultPayload = std::make_shared<MIXJoin>();
            for (auto node : nodes) {
                joinResultPayload->addSubscription(node);
            }
            joinResultPayload->setJID(JID("123456#coven@mix.shakespeare.example"));
            return IQ::createResult(ownJID_, channel_->sentStanzas[0]->getTo(), channel_->sentStanzas[0]->getID(), joinResultPayload);
        }

        JID ownJID_;
        DummyStanzaChannel* channel_;
        IQRouter* router_;
        XMPPRosterImpl* xmppRoster_;
        MIXRegistry* mixRegistry_;
        int successfulJoins_;
};

TEST_F(MIXRegistryTest, testJoinAndLeaveChannel) {
    auto testling = getMIXRegistry();
    testling->syncRegistryWithRoster();

    std::unordered_set<std::string> nodes;
    nodes.insert(std::string("urn:xmpp:mix:nodes:messages"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:presence"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:participants"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:config"));

    auto channelJID = JID("coven@mix.shakespeare.example");
    testling->joinChannel(channelJID, nodes);

    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXJoin>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXJoin>());
    ASSERT_FALSE(iq->getPayload<MIXJoin>()->getForm());
    ASSERT_EQ(static_cast<size_t>(4), iq->getPayload<MIXJoin>()->getSubscriptions().size());

    channel_->onIQReceived(createJoinResult(nodes));

    //Adding channel JID to roster post successful join
    xmppRoster_->addContact(channelJID, "coven", std::vector<std::string>(), RosterItemPayload::None, true);

    ASSERT_TRUE(mixRegistry_->getMIXInstance(channelJID));
    ASSERT_EQ(mixRegistry_->getChannels().size(), static_cast<int>(1));
    ASSERT_EQ(successfulJoins_, static_cast<int>(1));

    // sending join request for joined channel.
    testling->joinChannel(channelJID, nodes);
    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));

    //leave Channel
    testling->leaveChannel(channelJID);
    ASSERT_EQ(2, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXLeave>(1, ownJID_.toBare(), IQ::Set));

    //removing JID post successfully leaving channel.
    xmppRoster_->removeContact(channelJID);
    ASSERT_FALSE(mixRegistry_->getMIXInstance(channelJID));
    ASSERT_EQ(successfulJoins_, static_cast<int>(0));
    ASSERT_EQ(mixRegistry_->getChannels().size(), static_cast<int>(0));
}

TEST_F(MIXRegistryTest, testFailedJoin) {
    auto testling = getMIXRegistry();
    testling->syncRegistryWithRoster();

    std::unordered_set<std::string> nodes;
    nodes.insert(std::string("urn:xmpp:mix:nodes:messages"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:presence"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:participants"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:config"));

    auto channelJID = JID("coven@mix.shakespeare.example");
    testling->joinChannel(channelJID, nodes);

    channel_->onIQReceived(createError());
}
