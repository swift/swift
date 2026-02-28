/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Parser/PayloadParsers/MAMResultParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class MAMResultParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(MAMResultParserTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParse() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<result id=\"28482-98726-73623\" queryid=\"f27\" xmlns=\"urn:xmpp:mam:0\">"
                    "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                        "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                        "<message xmlns=\"jabber:client\" from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\">"
                            "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                        "</message>"
                    "</forwarded>"
                "</result>"));

            std::shared_ptr<MAMResult> payload = parser.getPayload<MAMResult>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT_EQUAL(std::string("28482-98726-73623"), payload->getID());
            CPPUNIT_ASSERT(payload->getQueryID());
            CPPUNIT_ASSERT_EQUAL(std::string("f27"), *payload->getQueryID());

            std::shared_ptr<Forwarded> forwarded = payload->getPayload();
            CPPUNIT_ASSERT(forwarded->getDelay());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-07-10T23:08:25Z"), dateTimeToString(forwarded->getDelay()->getStamp()));

            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(forwarded->getStanza());
            CPPUNIT_ASSERT(!!message);
            const std::string expectedBody = "Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.";
            CPPUNIT_ASSERT_EQUAL(expectedBody, message->getBody().get());
            CPPUNIT_ASSERT_EQUAL(Message::Chat, message->getType());
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), message->getTo());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), message->getFrom());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMResultParserTest);
