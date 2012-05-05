/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializers/ErrorSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Elements/Delay.h>

using namespace Swift;

class ErrorSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ErrorSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_Payload);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			ErrorSerializer testling(&serializers);
			boost::shared_ptr<ErrorPayload> error(new ErrorPayload(ErrorPayload::BadRequest, ErrorPayload::Cancel, "My Error"));

			CPPUNIT_ASSERT_EQUAL(std::string("<error type=\"cancel\"><bad-request xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/><text xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\">My Error</text></error>"), testling.serialize(error));
		}

		void testSerialize_Payload() {
			ErrorSerializer testling(&serializers);
			boost::shared_ptr<ErrorPayload> error = boost::make_shared<ErrorPayload>();
			error->setPayload(boost::make_shared<Delay>());

			CPPUNIT_ASSERT_EQUAL(std::string(
					"<error type=\"cancel\"><undefined-condition xmlns=\"urn:ietf:params:xml:ns:xmpp-stanzas\"/><delay stamp=\"not-a-date-timeZ\" xmlns=\"urn:xmpp:delay\"/></error>"
					), testling.serialize(error));
		}

	private:
		FullPayloadSerializerCollection serializers;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorSerializerTest);
