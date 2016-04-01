/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Parser/PayloadParsers/ForwardedParser.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

class ForwardedParserTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(ForwardedParserTest);
        CPPUNIT_TEST(testParseIQ);
        CPPUNIT_TEST(testParseMessage);
        CPPUNIT_TEST(testParseMessageNoDelay);
        CPPUNIT_TEST(testParsePresence);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testParseIQ() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay xmlns=\"urn:xmpp:delay\" stamp=\"2010-07-10T23:08:25Z\"/>"
                    "<iq xmlns=\"jabber:client\" type=\"get\" from=\"kindanormal@example.com/IM\" to=\"stupidnewbie@example.com\" id=\"id0\"/>"
                "</forwarded>"));

            std::shared_ptr<Forwarded> payload = parser.getPayload<Forwarded>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getDelay());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-07-10T23:08:25Z"), dateTimeToString(payload->getDelay()->getStamp()));

            std::shared_ptr<IQ> iq = std::dynamic_pointer_cast<IQ>(payload->getStanza());
            CPPUNIT_ASSERT(!!iq);
            CPPUNIT_ASSERT_EQUAL(JID("stupidnewbie@example.com"), iq->getTo());
            CPPUNIT_ASSERT_EQUAL(JID("kindanormal@example.com/IM"), iq->getFrom());
            CPPUNIT_ASSERT_EQUAL(std::string("id0"), iq->getID());
            CPPUNIT_ASSERT_EQUAL(IQ::Get, iq->getType());
        }

        void testParseMessage() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay xmlns=\"urn:xmpp:delay\" stamp=\"2010-07-10T23:08:25Z\"/>"
                    "<message xmlns=\"jabber:client\" to=\"juliet@capulet.lit/balcony\" from=\"romeo@montague.lit/orchard\" type=\"chat\">"
                        "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                    "</message>"
                "</forwarded>"));

            std::shared_ptr<Forwarded> payload = parser.getPayload<Forwarded>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getDelay());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-07-10T23:08:25Z"), dateTimeToString(payload->getDelay()->getStamp()));

            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(payload->getStanza());
            CPPUNIT_ASSERT(!!message);
            const std::string expectedBody = "Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.";
            CPPUNIT_ASSERT_EQUAL(expectedBody, message->getBody().get());
            CPPUNIT_ASSERT_EQUAL(Message::Chat, message->getType());
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), message->getTo());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), message->getFrom());
        }

        void testParseMessageNoDelay() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<message xmlns=\"jabber:client\" to=\"juliet@capulet.lit/balcony\" from=\"romeo@montague.lit/orchard\" type=\"chat\">"
                        "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                    "</message>"
                "</forwarded>"));

            std::shared_ptr<Forwarded> payload = parser.getPayload<Forwarded>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(!payload->getDelay());

            std::shared_ptr<Message> message = std::dynamic_pointer_cast<Message>(payload->getStanza());
            CPPUNIT_ASSERT(!!message);
            const std::string expectedBody = "Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.";
            CPPUNIT_ASSERT_EQUAL(expectedBody, message->getBody().get());
            CPPUNIT_ASSERT_EQUAL(Message::Chat, message->getType());
            CPPUNIT_ASSERT_EQUAL(JID("juliet@capulet.lit/balcony"), message->getTo());
            CPPUNIT_ASSERT_EQUAL(JID("romeo@montague.lit/orchard"), message->getFrom());
        }

        void testParsePresence() {
            PayloadsParserTester parser;
            CPPUNIT_ASSERT(parser.parse(
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay xmlns=\"urn:xmpp:delay\" stamp=\"2010-07-10T23:08:25Z\"/>"
                    "<presence xmlns=\"jabber:client\" from=\"alice@wonderland.lit/rabbithole\" to=\"madhatter@wonderland.lit\" type=\"unavailable\"/>"
                "</forwarded>"));

            std::shared_ptr<Forwarded> payload = parser.getPayload<Forwarded>();
            CPPUNIT_ASSERT(!!payload);
            CPPUNIT_ASSERT(payload->getDelay());
            CPPUNIT_ASSERT_EQUAL(std::string("2010-07-10T23:08:25Z"), dateTimeToString(payload->getDelay()->getStamp()));

            std::shared_ptr<Presence> presence = std::dynamic_pointer_cast<Presence>(payload->getStanza());
            CPPUNIT_ASSERT(!!presence);
            CPPUNIT_ASSERT_EQUAL(JID("madhatter@wonderland.lit"), presence->getTo());
            CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/rabbithole"), presence->getFrom());
            CPPUNIT_ASSERT_EQUAL(Presence::Unavailable, presence->getType());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ForwardedParserTest);
