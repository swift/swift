/*
 * Copyright (c) 2013 Tobias Markmann
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

#include <Swiften/Elements/BlockListPayload.h>
#include <Swiften/Elements/BlockPayload.h>
#include <Swiften/Elements/UnblockPayload.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Serializer/PayloadSerializers/BlockSerializer.h>

using namespace Swift;

class BlockSerializerTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(BlockSerializerTest);
        CPPUNIT_TEST(testExample4);
        CPPUNIT_TEST(testExample6);
        CPPUNIT_TEST(testExample10);
        CPPUNIT_TEST_SUITE_END();

    public:
        BlockSerializerTest() {}

        void testExample4() {
            BlockSerializer<BlockListPayload> testling("blocklist");
            std::shared_ptr<BlockListPayload> blocklist = std::make_shared<BlockListPayload>();
            blocklist->addItem(JID("romeo@montague.net"));
            blocklist->addItem(JID("iago@shakespeare.lit"));

            CPPUNIT_ASSERT_EQUAL(std::string("<blocklist xmlns=\"urn:xmpp:blocking\"><item jid=\"romeo@montague.net\"/><item jid=\"iago@shakespeare.lit\"/></blocklist>"), testling.serialize(blocklist));
        }

        void testExample6() {
            BlockSerializer<BlockPayload> testling("block");
            std::shared_ptr<BlockPayload> block = std::make_shared<BlockPayload>();
            block->addItem(JID("romeo@montague.net"));

            CPPUNIT_ASSERT_EQUAL(std::string("<block xmlns=\"urn:xmpp:blocking\"><item jid=\"romeo@montague.net\"/></block>"), testling.serialize(block));
        }

        void testExample10() {
            BlockSerializer<UnblockPayload> testling("unblock");
            std::shared_ptr<UnblockPayload> unblock = std::make_shared<UnblockPayload>();
            unblock->addItem(JID("romeo@montague.net"));

            CPPUNIT_ASSERT_EQUAL(std::string("<unblock xmlns=\"urn:xmpp:blocking\"><item jid=\"romeo@montague.net\"/></unblock>"), testling.serialize(unblock));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BlockSerializerTest);
