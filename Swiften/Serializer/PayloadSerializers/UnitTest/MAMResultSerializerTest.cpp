/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/MAMResult.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/MAMResultSerializer.h>

using namespace Swift;

class MAMResultSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(MAMResultSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize() {
            MAMResultSerializer serializer(&serializers);

            std::shared_ptr<Message> message(std::make_shared<Message>());
            message->setType(Message::Chat);
            message->setTo(JID("juliet@capulet.lit/balcony"));
            message->setFrom(JID("romeo@montague.lit/orchard"));
            message->setBody("Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.");

            std::shared_ptr<Forwarded> forwarded(std::make_shared<Forwarded>());
            forwarded->setStanza(message);
            forwarded->setDelay(std::make_shared<Delay>(stringToDateTime(std::string("2010-07-10T23:08:25Z"))));

            std::shared_ptr<MAMResult> result(std::make_shared<MAMResult>());
            result->setID("28482-98726-73623");
            result->setQueryID(std::string("f27"));
            result->setPayload(forwarded);

            std::string expectedResult =
                "<result id=\"28482-98726-73623\" queryid=\"f27\" xmlns=\"urn:xmpp:mam:0\">"
                    "<forwarded xmlns=\"urn:xmpp:forward:0\">"
                        "<delay stamp=\"2010-07-10T23:08:25Z\" xmlns=\"urn:xmpp:delay\"/>"
                        "<message from=\"romeo@montague.lit/orchard\" to=\"juliet@capulet.lit/balcony\" type=\"chat\" xmlns=\"jabber:client\">"
                            "<body>Call me but love, and I'll be new baptized; Henceforth I never will be Romeo.</body>"
                        "</message>"
                    "</forwarded>"
                "</result>";

            CPPUNIT_ASSERT_EQUAL(expectedResult, serializer.serialize(result));
        }

    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(MAMResultSerializerTest);
