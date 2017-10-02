/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Elements/Delay.h>
#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

class ErrorSerializerTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(ErrorSerializerTest);
        CPPUNIT_TEST(testSerialize);
        CPPUNIT_TEST(testSerialize_Payload);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testSerialize() {
            ErrorSerializer testling(&serializers);
            std::shared_ptr<ErrorPayload> error(new ErrorPayload(ErrorPayload::BadRequest, ErrorPayload::Cancel, "My Error"));

            CPPUNIT_ASSERT_EQUAL(std::string("<error type=\"cancel\"><bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/><text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">My Error</text></error>"), testling.serialize(error));
        }

        void testSerialize_Payload() {
            ErrorSerializer testling(&serializers);
            std::shared_ptr<ErrorPayload> error = std::make_shared<ErrorPayload>();
            error->setPayload(std::make_shared<Delay>());

            CPPUNIT_ASSERT_EQUAL(std::string(
                    "<error type=\"cancel\"><undefined-condition xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/><delay stamp=\"not-a-date-timeZ\" xmlns=\"urn:xmpp:delay\"/></error>"
                    ), testling.serialize(error));
        }

    private:
        FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorSerializerTest);
