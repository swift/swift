/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Serializer/AuthSuccessSerializer.h"
#include "Swiften/Elements/AuthSuccess.h"

using namespace Swift;

class AuthSuccessSerializerTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(AuthSuccessSerializerTest);
		CPPUNIT_TEST(testSerialize);
		CPPUNIT_TEST(testSerialize_NoMessage);
		CPPUNIT_TEST(testSerialize_EmptyMessage);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testSerialize() {
			AuthSuccessSerializer testling;
			boost::shared_ptr<AuthSuccess> authSuccess(new AuthSuccess());
			authSuccess->setValue("foo");

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
					"Zm9v"
				"</success>"), testling.serialize(authSuccess));
		}

		void testSerialize_NoMessage() {
			AuthSuccessSerializer testling;
			boost::shared_ptr<AuthSuccess> authSuccess(new AuthSuccess());

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
				"</success>"), testling.serialize(authSuccess));
		}

		void testSerialize_EmptyMessage() {
			AuthSuccessSerializer testling;
			boost::shared_ptr<AuthSuccess> authSuccess(new AuthSuccess());
			authSuccess->setValue(ByteArray());

			CPPUNIT_ASSERT_EQUAL(std::string(
				"<success xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\">"
					"="
				"</success>"), testling.serialize(authSuccess));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(AuthSuccessSerializerTest);
