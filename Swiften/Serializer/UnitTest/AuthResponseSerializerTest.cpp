/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/AuthResponseSerializer.h>
#include <Swiften/Elements/AuthResponse.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

class AuthResponseSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(AuthResponseSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoMessage);
		CPPUNIT_TEST(testSerialize_EmptyMessage);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			AuthResponseSerializer testling;
			boost::shared_ptr<AuthResponse> authResponse(new AuthResponse());
			authResponse->setValue(createSafeByteArray("foo"));

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
					"Zm9v"
				"</response>"), testling.serialize(authResponse));
		}

		void testSerialize_NoMessage() {
			AuthResponseSerializer testling;
			boost::shared_ptr<AuthResponse> authResponse(new AuthResponse());

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
				"</response>"), testling.serialize(authResponse));
		}

		void testSerialize_EmptyMessage() {
			AuthResponseSerializer testling;
			boost::shared_ptr<AuthResponse> authResponse(new AuthResponse());
			authResponse->setValue(SafeByteArray());

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<response xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
					"="
				"</response>"), testling.serialize(authResponse));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(AuthResponseSerializerTest);
