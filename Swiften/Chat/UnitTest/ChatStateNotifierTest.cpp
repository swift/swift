/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Chat/ChatStateNotifier.h>
#include <Swiften/Client/DummyStanzaChannel.h>
#include <Swiften/Disco/DummyEntityCapsProvider.h>

using namespace Swift;

class ChatStateNotifierTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatStateNotifierTest);
	CPPUNIT_TEST(testStartTypingReply_CapsNotIncluded);
	CPPUNIT_TEST(testStartTypingReply_CapsIncluded);
	CPPUNIT_TEST(testCancelledNewMessage);
	CPPUNIT_TEST(testContinueTypingReply_CapsIncluded);
	CPPUNIT_TEST(testTypeReplies_WentOffline);
	CPPUNIT_TEST(testContactShouldReceiveStates_CapsOnly);
	CPPUNIT_TEST(testContactShouldReceiveStates_CapsNorActive);
	CPPUNIT_TEST(testContactShouldReceiveStates_ActiveOverrideOn);	
	CPPUNIT_TEST(testContactShouldReceiveStates_ActiveOverrideOff);
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp() {
		stanzaChannel = new DummyStanzaChannel();
		stanzaChannel->setAvailable(true);
		entityCapsProvider = new DummyEntityCapsProvider();
		notifier_ = new ChatStateNotifier(stanzaChannel, JID("foo@bar.com/baz"), entityCapsProvider);
		notifier_->setContactIsOnline(true);
	}
	
	void tearDown() {
		delete notifier_;
		delete entityCapsProvider;
		delete stanzaChannel;
	}
	
	void testStartTypingReply_CapsNotIncluded() {
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(0, getComposingCount());
	}

	void testSendTwoMessages() {
		setContactHas85Caps();
		notifier_->setUserIsTyping();
		notifier_->userSentMessage();
		notifier_->setUserIsTyping();
		notifier_->userSentMessage();
		CPPUNIT_ASSERT_EQUAL(2, getComposingCount());
	}

	void testCancelledNewMessage() {
		setContactHas85Caps();
		notifier_->setUserIsTyping();
		notifier_->userCancelledNewMessage();
		CPPUNIT_ASSERT_EQUAL(1, getComposingCount());
		CPPUNIT_ASSERT_EQUAL(1, getActiveCount());
		CPPUNIT_ASSERT_EQUAL(ChatState::Active, stanzaChannel->sentStanzas[stanzaChannel->sentStanzas.size()-1]->getPayload<ChatState>()->getChatState());
	}


	void testContactShouldReceiveStates_CapsOnly() {
		setContactHas85Caps();
		boost::shared_ptr<Message> message(new Message());
		notifier_->addChatStateRequest(message);
		CPPUNIT_ASSERT(message->getPayload<ChatState>());
		CPPUNIT_ASSERT_EQUAL(ChatState::Active, message->getPayload<ChatState>()->getChatState());
	}

	void testContactShouldReceiveStates_CapsNorActive() {
		boost::shared_ptr<Message> message(new Message());
		notifier_->addChatStateRequest(message);
		CPPUNIT_ASSERT(!message->getPayload<ChatState>());
	}

	void testContactShouldReceiveStates_ActiveOverrideOn() {
		notifier_->receivedMessageFromContact(true);
		boost::shared_ptr<Message> message(new Message());
		notifier_->addChatStateRequest(message);
		CPPUNIT_ASSERT(message->getPayload<ChatState>());
		CPPUNIT_ASSERT_EQUAL(ChatState::Active, message->getPayload<ChatState>()->getChatState());
	}

	void testContactShouldReceiveStates_ActiveOverrideOff() {
		setContactHas85Caps();
		notifier_->receivedMessageFromContact(false);
		/* I originally read the MUST NOT send after receiving without Active and
		 * thought this should check for false, but I later found it was OPTIONAL
		 * (MAY) behaviour only for if you didn't receive caps.
		 */
		boost::shared_ptr<Message> message(new Message());
		notifier_->addChatStateRequest(message);
		CPPUNIT_ASSERT(message->getPayload<ChatState>());
		CPPUNIT_ASSERT_EQUAL(ChatState::Active, message->getPayload<ChatState>()->getChatState());
	}


	void testStartTypingReply_CapsIncluded() {
		setContactHas85Caps();
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(1, getComposingCount());
	}

	void testContinueTypingReply_CapsIncluded() {
		setContactHas85Caps();
		notifier_->setUserIsTyping();
		notifier_->setUserIsTyping();
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(1, getComposingCount());
		notifier_->userSentMessage();
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(2, getComposingCount());

	}

	void testTypeReplies_WentOffline() {
			setContactHas85Caps();
			notifier_->setUserIsTyping();
			CPPUNIT_ASSERT_EQUAL(1, getComposingCount());
			notifier_->setContactIsOnline(false);
			notifier_->userSentMessage();
			notifier_->setUserIsTyping();
			CPPUNIT_ASSERT_EQUAL(1, getComposingCount());
		}

	private:
		void setContactHas85Caps() {
			DiscoInfo::ref caps(new DiscoInfo());
			caps->addFeature(DiscoInfo::ChatStatesFeature);
			entityCapsProvider->caps[JID("foo@bar.com/baz")] = caps;
			entityCapsProvider->onCapsChanged(JID("foo@bar.com/baz"));
		}

		int getComposingCount() const {
			int result = 0;
			foreach(boost::shared_ptr<Stanza> stanza, stanzaChannel->sentStanzas) {
				if (stanza->getPayload<ChatState>() && stanza->getPayload<ChatState>()->getChatState() == ChatState::Composing) {
					result++;
				}
			}
			return result;
		}

		int getActiveCount() const {
			int result = 0;
			foreach(boost::shared_ptr<Stanza> stanza, stanzaChannel->sentStanzas) {
				if (stanza->getPayload<ChatState>() && stanza->getPayload<ChatState>()->getChatState() == ChatState::Active) {
					result++;
				}
			}
			return result;
		}

	private:
		DummyStanzaChannel* stanzaChannel;
		DummyEntityCapsProvider* entityCapsProvider;
		ChatStateNotifier* notifier_;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatStateNotifierTest);
