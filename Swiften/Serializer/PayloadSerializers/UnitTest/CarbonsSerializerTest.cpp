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
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Thread.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsDisableSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsEnableSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsPrivateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsReceivedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/CarbonsSentSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class CarbonsSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CarbonsSerializerTest);
        CPPUNIT_TEST(testSerializeExample3);
        CPPUNIT_TEST(testSerializeExample6);
        CPPUNIT_TEST(testSerializeExample12);
        CPPUNIT_TEST(testSerializeExample14);
        CPPUNIT_TEST(testSerializeExample15);
        CPPUNIT_TEST_SUITE_END();

    public:
        CarbonsSerializerTest() {}

        /*
         * Test serializing of example 3 in XEP-0280.
         */
        void testSerializeExample3() {
            CarbonsEnableSerializer testling;

            CPPUNIT_ASSERT_EQUAL(std::string("<enable xmlns=\"urn:xmpp:carbons:2\"/>"), testling.serialize(std::make_shared<CarbonsEnable>()));
        }

        /*
         * Test serializing of example 6 in XEP-0280.
         */
        void testSerializeExample6() {
            CarbonsDisableSerializer testling;

            CPPUNIT_ASSERT_EQUAL(std::string("<disable xmlns=\"urn:xmpp:carbons:2\"/>"), testling.serialize(std::make_shared<CarbonsDisable>()));
        }

        /*
         * Test serializing of example 12 in XEP-0280.
         */
        void testSerializeExample12() {
            CarbonsReceivedSerializer testling(&serializers);

            CarbonsReceived::ref received = std::make_shared<CarbonsReceived>();

            std::shared_ptr<Forwarded> forwarded = std::make_shared<Forwarded>();

            Message::ref message = std::make_shared<Message>();
            message->setFrom(JID("juliet@capulet.example/balcony"));
            message->setTo(JID("romeo@montague.example/garden"));
            message->setBody("What man art thou that, thus bescreen'd in night, so stumblest on my counsel?");
            message->addPayload(std::make_shared<Thread>("0e3141cd80894871a68e6fe6b1ec56fa"));

            forwarded->setStanza(message);
            received->setForwarded(forwarded);

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<received xmlns=\"urn:xmpp:carbons:2\">"
                    "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                        "<message from=\"juliet@capulet.example/balcony\""
                            " to=\"romeo@montague.example/garden\""
                            " type=\"chat\""
                            " xmlns=\"jabber:client\">"
                            "<body>What man art thou that, thus bescreen'd in night, so stumblest on my counsel?</body>"
                            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                        "</message>"
                    "</forwarded>"
                "</received>"), testling.serialize(received));
        }

        /*
         * Test serializing of example 14 in XEP-0280.
         */
        void testSerializeExample14() {
            CarbonsSentSerializer testling(&serializers);

            CarbonsSent::ref sent = std::make_shared<CarbonsSent>();

            std::shared_ptr<Forwarded> forwarded = std::make_shared<Forwarded>();

            Message::ref message = std::make_shared<Message>();
            message->setTo(JID("juliet@capulet.example/balcony"));
            message->setFrom(JID("romeo@montague.example/home"));
            message->setBody("Neither, fair saint, if either thee dislike.");
            message->addPayload(std::make_shared<Thread>("0e3141cd80894871a68e6fe6b1ec56fa"));

            forwarded->setStanza(message);
            sent->setForwarded(forwarded);

            CPPUNIT_ASSERT_EQUAL(std::string(
                "<sent xmlns=\"urn:xmpp:carbons:2\">"
                    "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                        "<message from=\"romeo@montague.example/home\""
                            " to=\"juliet@capulet.example/balcony\""
                            " type=\"chat\""
                            " xmlns=\"jabber:client\">"
                            "<body>Neither, fair saint, if either thee dislike.</body>"
                            "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                        "</message>"
                    "</forwarded>"
                "</sent>"), testling.serialize(sent));
        }

        /*
         * Test serializing of example 15 in XEP-0280.
         */
        void testSerializeExample15() {
            CarbonsPrivateSerializer testling;

            CPPUNIT_ASSERT_EQUAL(std::string("<private xmlns=\"urn:xmpp:carbons:2\"/>"), testling.serialize(std::make_shared<CarbonsPrivate>()));
        }
    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(CarbonsSerializerTest);
