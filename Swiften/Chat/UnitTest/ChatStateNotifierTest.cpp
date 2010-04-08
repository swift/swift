/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/bind.hpp>

#include "Swiften/Chat/ChatStateNotifier.h"

using namespace Swift;


class ChatStateMonitor {
public:
	ChatStateMonitor(ChatStateNotifier* notifier) {
		notifier_ = notifier;
		composingCallCount = 0;
		activeCallCount = 0;
		notifier->onChatStateChanged.connect(boost::bind(&ChatStateMonitor::handleChatStateChanged, this, _1));
	};

	int composingCallCount;
	int activeCallCount;
	ChatState::ChatStateType currentState;

private:
	void handleChatStateChanged(ChatState::ChatStateType newState) {
		switch (newState) {
			case ChatState::Composing:
				composingCallCount++;
				break;
			case ChatState::Active:
				activeCallCount++;
				break;
			default:
				break;
			}
		currentState = newState;
	};

	ChatStateNotifier* notifier_;
};

class ChatStateNotifierTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ChatStateNotifierTest);
	CPPUNIT_TEST(testStartTypingReply_CapsNotIncluded);
	CPPUNIT_TEST(testStartTypingReply_CapsIncluded);
	CPPUNIT_TEST(testCancelledNewMessage);
	CPPUNIT_TEST(testContinueTypingReply_CapsIncluded);
	CPPUNIT_TEST(testContactShouldReceiveStates_CapsOnly);
	CPPUNIT_TEST(testContactShouldReceiveStates_CapsNorActive);
	CPPUNIT_TEST(testContactShouldReceiveStates_ActiveOverrideOn);	
	CPPUNIT_TEST(testContactShouldReceiveStates_ActiveOverrideOff);
	CPPUNIT_TEST_SUITE_END();
	
private:
	ChatStateNotifier* notifier_;
	ChatStateMonitor* monitor_;
	
public:
	void setUp() {
		notifier_ = new ChatStateNotifier();
		monitor_ = new ChatStateMonitor(notifier_);
	}
	
	void tearDown() {
		delete notifier_;
		delete monitor_;
	}
	
	void testStartTypingReply_CapsNotIncluded() {
		notifier_->setContactHas85Caps(false);
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(0, monitor_->composingCallCount);
	}

	void testSendTwoMessages() {
		notifier_->setContactHas85Caps(true);
		notifier_->setUserIsTyping();
		notifier_->userSentMessage();
		notifier_->setUserIsTyping();
		notifier_->userSentMessage();
		CPPUNIT_ASSERT_EQUAL(2, monitor_->composingCallCount);
	}

	void testCancelledNewMessage() {
		notifier_->setContactHas85Caps(true);
		notifier_->setUserIsTyping();
		notifier_->userCancelledNewMessage();
		CPPUNIT_ASSERT_EQUAL(1, monitor_->composingCallCount);
		CPPUNIT_ASSERT_EQUAL(1, monitor_->activeCallCount);
		CPPUNIT_ASSERT_EQUAL(ChatState::Active, monitor_->currentState);
	}


	void testContactShouldReceiveStates_CapsOnly() {
		notifier_->setContactHas85Caps(true);
		CPPUNIT_ASSERT_EQUAL(true, notifier_->contactShouldReceiveStates());
	}

	void testContactShouldReceiveStates_CapsNorActive() {
		CPPUNIT_ASSERT_EQUAL(false, notifier_->contactShouldReceiveStates());
	}

	void testContactShouldReceiveStates_ActiveOverrideOn() {
		notifier_->setContactHas85Caps(false);
		notifier_->receivedMessageFromContact(true);
		CPPUNIT_ASSERT_EQUAL(true, notifier_->contactShouldReceiveStates());
	}

	void testContactShouldReceiveStates_ActiveOverrideOff() {
		notifier_->setContactHas85Caps(true);
		notifier_->receivedMessageFromContact(false);
		CPPUNIT_ASSERT_EQUAL(false, notifier_->contactShouldReceiveStates());
	}


	void testStartTypingReply_CapsIncluded() {
		notifier_->setContactHas85Caps(true);
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(1, monitor_->composingCallCount);
	}

	void testContinueTypingReply_CapsIncluded() {
		notifier_->setContactHas85Caps(true);
		notifier_->setUserIsTyping();
		notifier_->setUserIsTyping();
		notifier_->setUserIsTyping();
		CPPUNIT_ASSERT_EQUAL(1, monitor_->composingCallCount);
	}

	
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatStateNotifierTest);
