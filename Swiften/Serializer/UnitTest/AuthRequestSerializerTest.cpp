/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Serializer/AuthRequestSerializer.h>
#include <Swiften/Elements/AuthRequest.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

class AuthRequestSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(AuthRequestSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoMessage);
		CPPUNIT_TEST(testSerialize_EmptyMessage);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			AuthRequestSerializer testling;
			boost::shared_ptr<AuthRequest> authRequest(new AuthRequest("PLAIN"));
			authRequest->setMessage(createSafeByteArray("foo"));

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
					"Zm9v"
				"</auth>"), testling.serialize(authRequest));
		}

		void testSerialize_NoMessage() {
			AuthRequestSerializer testling;
			boost::shared_ptr<AuthRequest> authRequest(new AuthRequest("PLAIN"));

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
				"</auth>"), testling.serialize(authRequest));
		}

		void testSerialize_EmptyMessage() {
			AuthRequestSerializer testling;
			boost::shared_ptr<AuthRequest> authRequest(new AuthRequest("PLAIN"));
			authRequest->setMessage(SafeByteArray());

			CPPUNIT_ASSERT_EQUAL(createSafeByteArray(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
					"="
				"</auth>"), testling.serialize(authRequest));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(AuthRequestSerializerTest);
