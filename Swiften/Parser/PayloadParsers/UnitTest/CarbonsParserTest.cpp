/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/CarbonsDisable.h>
#include <Swiften/Elements/CarbonsEnable.h>
#include <Swiften/Elements/CarbonsPrivate.h>
#include <Swiften/Elements/CarbonsReceived.h>
#include <Swiften/Elements/CarbonsSent.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Thread.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class CarbonsParserTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CarbonsParserTest);
        CPPUNIT_TEST(testParseExample3);
        CPPUNIT_TEST(testParseExample6);
        CPPUNIT_TEST(testParseExample12);
        CPPUNIT_TEST(testParseExample14);
        CPPUNIT_TEST(testParseExample15);
        CPPUNIT_TEST_SUITE_END();

    public:
        CarbonsParserTest() {}

        /*
         * Test parsing of example 3 in XEP-0280.
         */
        void testParseExample3() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse("<enable xmlns='urn:xmpp:carbons:2' />"));

            CarbonsEnable::ref enable = parser.getPayload<CarbonsEnable>();
            CPPUNIT_ASSERT(enable);
        }

        /*
         * Test parsing of example 6 in XEP-0280.
         */
        void testParseExample6() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse("<disable xmlns='urn:xmpp:carbons:2' />"));

            CarbonsDisable::ref disable = parser.getPayload<CarbonsDisable>();
            CPPUNIT_ASSERT(disable);
        }

        /*
         * Test parsing of example 12 in XEP-0280.
         */
        void testParseExample12() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse("<received xmlns='urn:xmpp:carbons:2'>"
                                            "<forwarded xmlns='urn:xmpp:forward:0'>"
                                                "<message xmlns='jabber:client'"
                                                    " from='juliet@capulet.example/balcony'"
                                                    " to='romeo@montague.example/garden'"
                                                    " type='chat'>"
                                                    "<body>What man art thou that, thus bescreen'd in night, so stumblest on my counsel?</body>"
                                                    "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                                                "</message>"
                                            "</forwarded>"
                                        "</received>"));

            CarbonsReceived::ref received = parser.getPayload<CarbonsReceived>();
            CPPUNIT_ASSERT(received);

            std::shared_ptr<Forwarded> forwarded = received->getForwarded();
            CPPUNIT_ASSERT(forwarded);

            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(forwarded->getStanza());
            CPPUNIT_ASSERT(message);
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.example/balcony"), message->getFrom());

            std::shared_ptr<Thread> thread = message->getPayload<Thread>();
            CPPUNIT_ASSERT(thread);
            CPPUNIT_ASSERT_EQUAL(std::string("0e3141cd80894871a68e6fe6b1ec56fa"), thread->getText());
        }

        /*
         * Test parsing of example 14 in XEP-0280.
         */
        void testParseExample14() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse("<sent xmlns='urn:xmpp:carbons:2'>"
                                            "<forwarded xmlns='urn:xmpp:forward:0'>"
                                                "<message xmlns='jabber:client'"
                                                    " to='juliet@capulet.example/balcony'"
                                                    " from='romeo@montague.example/home'"
                                                    " type='chat'>"
                                                    "<body>Neither, fair saint, if either thee dislike.</body>"
                                                    "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                                                "</message>"
                                            "</forwarded>"
                                        "</sent>"));

            CarbonsSent::ref sent = parser.getPayload<CarbonsSent>();
            CPPUNIT_ASSERT(sent);

            std::shared_ptr<Forwarded> forwarded = sent->getForwarded();
            CPPUNIT_ASSERT(forwarded);

            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(forwarded->getStanza());
            CPPUNIT_ASSERT(message);
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.example/balcony"), message->getTo());
        }

        /*
         * Test parsing of example 15 in XEP-0280.
         */
        void testParseExample15() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse("<private xmlns='urn:xmpp:carbons:2'/>"));

            CPPUNIT_ASSERT(parser.getPayload<CarbonsPrivate>());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CarbonsParserTest);
