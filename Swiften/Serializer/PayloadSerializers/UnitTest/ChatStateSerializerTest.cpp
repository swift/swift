/*
 * Copyright (c) 2015 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
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
        CPPUNIT_TEST(testSerialize_ActiveState);
        CPPUNIT_TEST(testSerialize_GoneState);
        CPPUNIT_TEST(testSerialize_ComposingState);
        CPPUNIT_TEST(testSerialize_PausedState);
        CPPUNIT_TEST(testSerialize_InacativeState);
        CPPUNIT_TEST_SUITE_END();

    public:
        ChatStateSerializerTest() {}
        ChatStateSerializer testling;

        void testSerialize_ActiveState() {
            std::shared_ptr<ChatState> priority(new ChatState(ChatState::Active));
            CPPUNIT_ASSERT_EQUAL(std::string("<active xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
        }

        void testSerialize_GoneState() {
            std::shared_ptr<ChatState> priority(new ChatState(ChatState::Gone));
            CPPUNIT_ASSERT_EQUAL(std::string("<gone xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
        }

        void testSerialize_ComposingState() {
            std::shared_ptr<ChatState> priority(new ChatState(ChatState::Composing));
            CPPUNIT_ASSERT_EQUAL(std::string("<composing xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
        }

        void testSerialize_PausedState() {
            std::shared_ptr<ChatState> priority(new ChatState(ChatState::Paused));
            CPPUNIT_ASSERT_EQUAL(std::string("<paused xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
        }

        void testSerialize_InacativeState() {
            std::shared_ptr<ChatState> priority(new ChatState(ChatState::Inactive));
            CPPUNIT_ASSERT_EQUAL(std::string("<inactive xmlns=\"http://jabber.org/protocol/chatstates\"/>"), testling.serialize(priority));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ChatStateSerializerTest);
