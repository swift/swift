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
#include <Swiften/Parser/PayloadParsers/BlockParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class BlockParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(BlockParserTest);
        CPPUNIT_TEST(testExample4);
        CPPUNIT_TEST(testExample6);
        CPPUNIT_TEST(testExample10);
        CPPUNIT_TEST_SUITE_END();

    public:
        BlockParserTest() {}

        void testExample4() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse("<blocklist xmlns='urn:xmpp:blocking'>"
                                            "<item jid='romeo@montague.net'/>"
                                            "<item jid='iago@shakespeare.lit'/>"
                                        "</blocklist>"));

            BlockListPayload* payload = dynamic_cast<BlockListPayload*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(2 == payload->getItems().size());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), payload->getItems()[0]);
            CPPUNIT_ASSERT_EQUAL(JID("iago@shakespeare.lit"), payload->getItems()[1]);
        }

        void testExample6() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse("<block xmlns='urn:xmpp:blocking'>"
                                            "<item jid='romeo@montague.net'/>"
                                        "</block>"));

            BlockPayload* payload = dynamic_cast<BlockPayload*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(1 == payload->getItems().size());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), payload->getItems()[0]);
        }

        void testExample10() {
            PayloadsParserTester parser;

            CPPUNIT_ASSERT(parser.parse("<unblock xmlns='urn:xmpp:blocking'>"
                                            "<item jid='romeo@montague.net'/>"
                                        "</unblock>"));

            UnblockPayload* payload = dynamic_cast<UnblockPayload*>(parser.getPayload().get());
            CPPUNIT_ASSERT(payload);
            CPPUNIT_ASSERT(1 == payload->getItems().size());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.net"), payload->getItems()[0]);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BlockParserTest);
