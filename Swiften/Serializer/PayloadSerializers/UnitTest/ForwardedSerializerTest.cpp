/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class ForwardedSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ForwardedSerializerTest);
        CPPUNIT_TEST(testSerializeIQ);
        CPPUNIT_TEST(testSerializeMessage);
        CPPUNIT_TEST(testSerializeMessageNoDelay);
        CPPUNIT_TEST(testSerializePresence);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerializeIQ() {
            ForwardedSerializer serializer(&serializers);

            boost::shared_ptr<IQ> iq = IQ::createResult(JID("juliet@capulet.lit/balcony"), JID("romeo@montague.lit/orchard"), "id0");

            boost::shared_ptr<Forwarded> forwarded(boost::make_shared<Forwarded>());
            forwarded->setStanza(iq);
            forwarded->setDelay(boost::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));

            std::string expectedResult =
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                    "<iq from=\"romeo@montague.lit/orchard\" id=\"id0\" to=\"juliet@capulet.lit/balcony\" type=\"result\" xmlns=\"jabber:client\"/>"
                "</forwarded>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(forwarded));
        }

        void testSerializeMessage() {
            ForwardedSerializer serializer(&serializers);

            boost::shared_ptr<Message> message(boost::make_shared<Message>());
            message->setType(Message::Chat);
            message->setTo(JID("juliet@capulet.lit/balcony"));
            message->setFrom(JID("romeo@montague.lit/orchard"));
            message->setBody("Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.");

            boost::shared_ptr<Forwarded> forwarded(boost::make_shared<Forwarded>());
            forwarded->setStanza(message);
            forwarded->setDelay(boost::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));

            std::string expectedResult =
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                    "<message from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\" xmlns=\"jabber:client\">"
                        "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                    "</message>"
                "</forwarded>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(forwarded));
        }

        void testSerializeMessageNoDelay() {
            ForwardedSerializer serializer(&serializers);

            boost::shared_ptr<Message> message(boost::make_shared<Message>());
            message->setType(Message::Chat);
            message->setTo(JID("juliet@capulet.lit/balcony"));
            message->setFrom(JID("romeo@montague.lit/orchard"));
            message->setBody("Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.");

            boost::shared_ptr<Forwarded> forwarded(boost::make_shared<Forwarded>());
            forwarded->setStanza(message);

            std::string expectedResult =
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<message from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\" xmlns=\"jabber:client\">"
                        "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                    "</message>"
                "</forwarded>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(forwarded));
        }

        void testSerializePresence() {
            ForwardedSerializer serializer(&serializers);

            boost::shared_ptr<Presence> presence(boost::make_shared<Presence>());
            presence->setType(Presence::Subscribe);

            boost::shared_ptr<Forwarded> forwarded(boost::make_shared<Forwarded>());
            forwarded->setStanza(presence);
            forwarded->setDelay(boost::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));

            std::string expectedResult =
                "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                    "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                    "<presence type=\"subscribe\" xmlns=\"jabber:client\"/>"
                "</forwarded>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(forwarded));
        }

    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ForwardedSerializerTest);
