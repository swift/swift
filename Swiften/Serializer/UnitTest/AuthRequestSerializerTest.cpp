/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/AuthRequestSerializer.h"
#include "Swiften/Elements/AuthRequest.h"

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
			authRequest->setMessage("foo");

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
					"Zm9v"
				"</auth>"), testling.serialize(authRequest));
		}

		void testSerialize_NoMessage() {
			AuthRequestSerializer testling;
			boost::shared_ptr<AuthRequest> authRequest(new AuthRequest("PLAIN"));

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
				"</auth>"), testling.serialize(authRequest));
		}

		void testSerialize_EmptyMessage() {
			AuthRequestSerializer testling;
			boost::shared_ptr<AuthRequest> authRequest(new AuthRequest("PLAIN"));
			authRequest->setMessage(ByteArray());

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">"
					"="
				"</auth>"), testling.serialize(authRequest));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(AuthRequestSerializerTest);
