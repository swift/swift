/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <boost/bind.hpp>

#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/MIX/MIXImpl.h>
#include <Swiften/Queries/IQRouter.h>

using namespace Swift;

class MIXImplTest : public ::testing::Test {

    protected:
        void SetUp() {
            ownJID_ = JID("hag66@shakespeare.example/UUID-a1j/7533");
            channelJID_ = JID("coven@mix.shakespeare.example");
            channel_ = new DummyStanzaChannel();
            router_ = new IQRouter(channel_);
            successfulJoins_ = 0;
        }

        void TearDown() {
           delete router_;
           delete channel_;
        }

        MIX::ref createMIXClient() {
            auto mix = std::make_shared<MIXImpl>(ownJID_, channelJID_, router_);
            mix->onJoinComplete.connect(boost::bind(&MIXImplTest::handleJoinComplete, this, _1));
            mix->onLeaveComplete.connect(boost::bind(&MIXImplTest::handleLeaveComplete, this, _1));
            return mix;
        }

        void handleJoinComplete(MIXJoin::ref joinPayload) {
            ASSERT_TRUE(joinPayload);
            ASSERT_TRUE(joinPayload->getJID());
            ASSERT_EQ(*joinPayload->getJID(), JID("123456#coven@mix.shakespeare.example"));
            ++successfulJoins_;
            subscribedNodes_ = joinPayload->getSubscriptions();
        }

        void handleLeaveComplete(MIXLeave::ref leavePayload) {
            ASSERT_TRUE(leavePayload);
            ASSERT_EQ(static_cast<int>(0), subscribedNodes_.size());
        }

        IQ::ref createJoinResult(const std::unordered_set<std::string>& nodes) {
            auto joinResultPayload = std::make_shared<MIXJoin>();
            for (auto node : nodes) {
                joinResultPayload->addSubscription(node);
            }
            joinResultPayload->setJID(JID("123456#coven@mix.shakespeare.example"));
            return IQ::createResult(ownJID_, channel_->sentStanzas[0]->getTo(), channel_->sentStanzas[0]->getID(), joinResultPayload);
        }

        IQ::ref createLeaveResult() {
            auto leaveResultPayload = std::make_shared<MIXLeave>();
            return IQ::createResult(ownJID_, channel_->sentStanzas[0]->getTo(), channel_->sentStanzas[0]->getID(), leaveResultPayload);
        }

        IQ::ref createJoinError() {
            return IQ::createError(ownJID_, channel_->sentStanzas[0]->getTo(), channel_->sentStanzas[0]->getID());
        }

        bool hasSubscription(const std::string& value) {
            return std::find(subscribedNodes_.begin(), subscribedNodes_.end(), value) != subscribedNodes_.end();
        }

        JID ownJID_;
        JID channelJID_;
        DummyStanzaChannel* channel_;
        IQRouter* router_;
        int successfulJoins_;
        std::unordered_set<std::string> subscribedNodes_;
};

TEST_F(MIXImplTest, testJoinChannelOnly) {
    MIX::ref testling = createMIXClient();
    testling->joinChannel();

    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXJoin>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXJoin>());
    ASSERT_FALSE(iq->getPayload<MIXJoin>()->getForm());
    ASSERT_EQ(static_cast<size_t>(0), iq->getPayload<MIXJoin>()->getSubscriptions().size());

    channel_->onIQReceived(createJoinResult(std::unordered_set<std::string>()));
    ASSERT_EQ(static_cast<int>(1), successfulJoins_);
    ASSERT_EQ(static_cast<int>(0), subscribedNodes_.size());
}

TEST_F(MIXImplTest, testJoinError) {
    MIX::ref testling = createMIXClient();
    testling->joinChannel();

    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXJoin>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXJoin>());
    ASSERT_FALSE(iq->getPayload<MIXJoin>()->getForm());
    ASSERT_EQ(static_cast<size_t>(0), iq->getPayload<MIXJoin>()->getSubscriptions().size());

    channel_->onIQReceived(createJoinError());
    ASSERT_EQ(static_cast<int>(0), successfulJoins_);
    ASSERT_EQ(static_cast<int>(0), subscribedNodes_.size());
}

TEST_F(MIXImplTest, testJoinWithAllSubscriptions) {
    MIX::ref testling = createMIXClient();
    std::unordered_set<std::string> nodes;
    nodes.insert(std::string("urn:xmpp:mix:nodes:messages"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:presence"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:participants"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:config"));

    testling->joinChannelAndSubscribe(nodes);

    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXJoin>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXJoin>());
    ASSERT_FALSE(iq->getPayload<MIXJoin>()->getForm());
    ASSERT_EQ(static_cast<size_t>(4), iq->getPayload<MIXJoin>()->getSubscriptions().size());

    channel_->onIQReceived(createJoinResult(nodes));
    ASSERT_EQ(static_cast<int>(1), successfulJoins_);
    ASSERT_EQ(static_cast<int>(4), subscribedNodes_.size());
}

TEST_F(MIXImplTest, testJoinWithSomeSubscriptions) {
    MIX::ref testling = createMIXClient();
    std::unordered_set<std::string> nodes;
    nodes.insert(std::string("urn:xmpp:mix:nodes:messages"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:presence"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:participants"));
    nodes.insert(std::string("urn:xmpp:mix:nodes:config"));

    testling->joinChannelAndSubscribe(nodes);

    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXJoin>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXJoin>());
    ASSERT_FALSE(iq->getPayload<MIXJoin>()->getForm());
    ASSERT_EQ(static_cast<size_t>(4), iq->getPayload<MIXJoin>()->getSubscriptions().size());

    std::unordered_set<std::string> subscribedTo;
    subscribedTo.insert(std::string("urn:xmpp:mix:nodes:messages"));

    channel_->onIQReceived(createJoinResult(subscribedTo));
    ASSERT_EQ(static_cast<int>(1), successfulJoins_);
    ASSERT_EQ(static_cast<int>(1), subscribedNodes_.size());
    ASSERT_TRUE(hasSubscription(std::string("urn:xmpp:mix:nodes:messages")));
}


TEST_F(MIXImplTest, testLeaveChannel) {
    MIX::ref testling = createMIXClient();
    testling->leaveChannel();
    ASSERT_EQ(1, static_cast<int>(channel_->sentStanzas.size()));
    ASSERT_TRUE(channel_->isRequestAtIndex<MIXLeave>(0, ownJID_.toBare(), IQ::Set));

    auto iq = channel_->getStanzaAtIndex<IQ>(0);
    ASSERT_TRUE(iq);
    ASSERT_TRUE(iq->getPayload<MIXLeave>());
    ASSERT_TRUE(iq->getPayload<MIXLeave>()->getChannel());

    channel_->onIQReceived(createLeaveResult());
}
