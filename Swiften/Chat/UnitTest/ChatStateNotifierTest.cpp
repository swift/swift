/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/bind.hpp>

#include <gtest/gtest.h>

#include <Swiften/Chat/ChatStateNotifier.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>
#include <Swiften/Network/DummyTimerFactory.h>

using namespace Swift;

class ChatStateNotifierTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        stanzaChannel = new DummyStanzaChannel();
        stanzaChannel->setAvailable(true);
        entityCapsProvider = new DummyEntityCapsProvider();
        timerFactory_ = new DummyTimerFactory();
        notifier_ = new ChatStateNotifier(stanzaChannel, JID("foo@bar.com/baz"), entityCapsProvider, timerFactory_, 2);
        notifier_->setContactIsOnline(true);
    }

    virtual void TearDown() {
        delete notifier_;
        delete timerFactory_;
        delete entityCapsProvider;
        delete stanzaChannel;
    }

    void setContactHas85Caps() {
        DiscoInfo::ref caps(new DiscoInfo());
        caps->addFeature(DiscoInfo::ChatStatesFeature);
        entityCapsProvider->caps[JID("foo@bar.com/baz")] = caps;
        entityCapsProvider->onCapsChanged(JID("foo@bar.com/baz"));
    }

    int getComposingCount() const {
        int result = 0;
        for (auto&& stanza : stanzaChannel->sentStanzas) {
            if (stanza->getPayload<ChatState>() && stanza->getPayload<ChatState>()->getChatState() == ChatState::Composing) {
                result++;
            }
        }
        return result;
    }

    int getActiveCount() const {
        int result = 0;
        for (auto&& stanza : stanzaChannel->sentStanzas) {
            if (stanza->getPayload<ChatState>() && stanza->getPayload<ChatState>()->getChatState() == ChatState::Active) {
                result++;
            }
        }
        return result;
    }

    DummyStanzaChannel* stanzaChannel;
    DummyEntityCapsProvider* entityCapsProvider;
    DummyTimerFactory* timerFactory_;
    ChatStateNotifier* notifier_;
};

TEST_F(ChatStateNotifierTest, testStartTypingReply_CapsNotIncluded) {
    notifier_->setUserIsTyping();
    ASSERT_EQ(0, getComposingCount());
}

TEST_F(ChatStateNotifierTest, testSendTwoMessages) {
    setContactHas85Caps();
    notifier_->setUserIsTyping();
    notifier_->userSentMessage();
    notifier_->setUserIsTyping();
    notifier_->userSentMessage();
    ASSERT_EQ(2, getComposingCount());
}

TEST_F(ChatStateNotifierTest, testCancelledNewMessage) {
    setContactHas85Caps();
    notifier_->setUserIsTyping();
    notifier_->userCancelledNewMessage();
    ASSERT_EQ(1, getComposingCount());
    ASSERT_EQ(1, getActiveCount());
    ASSERT_EQ(ChatState::Active, stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size() - 1]->getPayload<ChatState>()->getChatState());
}

TEST_F(ChatStateNotifierTest, testIdleWhileTypingNewMessage) {
    setContactHas85Caps();
    //The channel should be empty
    ASSERT_EQ(0, getComposingCount());
    ASSERT_EQ(0, getActiveCount());
    notifier_->setUserIsTyping();
    timerFactory_->setTime(1);
    //1 Composing stanza is expected
    ASSERT_EQ(1, getComposingCount());
    ASSERT_EQ(0, getActiveCount());
    timerFactory_->setTime(2);
    //The idleTimer period has expired, the channel should have 1 composing and 1 active status stanza
    ASSERT_EQ(1, getComposingCount());
    ASSERT_EQ(1, getActiveCount());
    ASSERT_EQ(ChatState::Active, stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size() - 1]->getPayload<ChatState>()->getChatState());
    timerFactory_->setTime(4);
    //At the second tick no further state stanzas should be sent.
    ASSERT_EQ(1, getComposingCount());
    ASSERT_EQ(1, getActiveCount());
    ASSERT_EQ(ChatState::Active, stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size() - 1]->getPayload<ChatState>()->getChatState());
}

TEST_F(ChatStateNotifierTest, testIdleWhileTypingNewMessageNoCaps) {
    notifier_->setUserIsTyping();
    timerFactory_->setTime(3);
    ASSERT_EQ(0, getComposingCount());
    ASSERT_EQ(0, getActiveCount());
}
TEST_F(ChatStateNotifierTest, testContactShouldReceiveStates_CapsOnly) {
    setContactHas85Caps();
    std::shared_ptr<Message> message(new Message());
    notifier_->addChatStateRequest(message);
    EXPECT_TRUE(message->getPayload<ChatState>());
    ASSERT_EQ(ChatState::Active, message->getPayload<ChatState>()->getChatState());
}

TEST_F(ChatStateNotifierTest, testContactShouldReceiveStates_CapsNorActive) {
    std::shared_ptr<Message> message(new Message());
    notifier_->addChatStateRequest(message);
    EXPECT_TRUE(!message->getPayload<ChatState>());
}

TEST_F(ChatStateNotifierTest, testContactShouldReceiveStates_ActiveOverrideOn) {
    notifier_->receivedMessageFromContact(true);
    std::shared_ptr<Message> message(new Message());
    notifier_->addChatStateRequest(message);
    EXPECT_TRUE(message->getPayload<ChatState>());
    ASSERT_EQ(ChatState::Active, message->getPayload<ChatState>()->getChatState());
}

TEST_F(ChatStateNotifierTest, testContactShouldReceiveStates_ActiveOverrideOff) {
    setContactHas85Caps();
    notifier_->receivedMessageFromContact(false);
    /* I originally read the MUST NOT send after receiving without Active and
    * thought this should check for false, but I later found it was OPTIONAL
    * (MAY) behaviour only for if you didn't receive caps.
    */
    std::shared_ptr<Message> message(new Message());
    notifier_->addChatStateRequest(message);
    EXPECT_TRUE(message->getPayload<ChatState>());
    ASSERT_EQ(ChatState::Active, message->getPayload<ChatState>()->getChatState());
}


TEST_F(ChatStateNotifierTest, testStartTypingReply_CapsIncluded) {
    setContactHas85Caps();
    notifier_->setUserIsTyping();
    ASSERT_EQ(1, getComposingCount());
}

TEST_F(ChatStateNotifierTest, testContinueTypingReply_CapsIncluded) {
    setContactHas85Caps();
    notifier_->setUserIsTyping();
    notifier_->setUserIsTyping();
    notifier_->setUserIsTyping();
    ASSERT_EQ(1, getComposingCount());
    notifier_->userSentMessage();
    notifier_->setUserIsTyping();
    ASSERT_EQ(2, getComposingCount());

}

TEST_F(ChatStateNotifierTest, testTypeReplies_WentOffline) {
    setContactHas85Caps();
    notifier_->setUserIsTyping();
    ASSERT_EQ(1, getComposingCount());
    notifier_->setContactIsOnline(false);
    notifier_->userSentMessage();
    notifier_->setUserIsTyping();
    ASSERT_EQ(1, getComposingCount());
}
