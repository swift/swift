/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/PayloadSerializers/ChatStateSerializer.h>

using namespace Swift;

class ChatStateSerializerTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(ChatStateSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST_SUITE_END();

	public:
		ChatStateSerializerTest() {}
		ChatStateSerializer testling;

		void testSerialize() {
			testGoneState();
			testComposingState();
			testPausedState();
			testInactiveState();
			testActiveState();	
		}

		void testGoneState() {
			boost::shared_ptr<ChatState> priority(new ChatState(ChatState::Gone));
			CPPUNIT_ASSERT_EQUAL(std::string("<gone xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
		}

		void testComposingState() {
			boost::shared_ptr<ChatState> priority(new ChatState(ChatState::Composing));
			CPPUNIT_ASSERT_EQUAL(std::string("<composing xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
		}

		void testPausedState() {
			boost::shared_ptr<ChatState> priority(new ChatState(ChatState::Paused));
			CPPUNIT_ASSERT_EQUAL(std::string("<paused xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
		}

		void testInactiveState() {
			boost::shared_ptr<ChatState> priority(new ChatState(ChatState::Inactive));
			CPPUNIT_ASSERT_EQUAL(std::string("<inactive xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
		}

		void testActiveState() {
			boost::shared_ptr<ChatState> priority(new ChatState(ChatState::Active));
			CPPUNIT_ASSERT_EQUAL(std::string("<active xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));	
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatStateSerializerTest);
