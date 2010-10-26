/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/PLAINClientAuthenticator.h"

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

using namespace Swift;

class PLAINClientAuthenticatorTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PLAINClientAuthenticatorTest);
		CPPUNIT_TEST(testGetResponse_WithoutAuthzID);
		CPPUNIT_TEST(testGetResponse_WithAuthzID);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetResponse_WithoutAuthzID() {
			PLAINClientAuthenticator testling;

			testling.setCredentials("user", "pass");

			CPPUNIT_ASSERT_EQUAL(*testling.getResponse(), ByteArray("\0user\0pass", 10));
		}

		void testGetResponse_WithAuthzID() {
			PLAINClientAuthenticator testling;

			testling.setCredentials("user", "pass", "authz");

			CPPUNIT_ASSERT_EQUAL(*testling.getResponse(), ByteArray("authz\0user\0pass", 15));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PLAINClientAuthenticatorTest);
