#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/SASL/PLAINClientAuthenticator.h"

using namespace Swift;

class PLAINClientAuthenticatorTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PLAINClientAuthenticatorTest);
		CPPUNIT_TEST(testGetResponse_WithoutAuthzID);
		CPPUNIT_TEST(testGetResponse_WithAuthzID);
		CPPUNIT_TEST_SUITE_END();

	public:
		PLAINClientAuthenticatorTest() {}

		void testGetResponse_WithoutAuthzID() {
			PLAINClientAuthenticator testling;

			testling.setCredentials("user", "pass");

			CPPUNIT_ASSERT_EQUAL(testling.getResponse(), ByteArray("\0user\0pass", 10));
		}

		void testGetResponse_WithAuthzID() {
			PLAINClientAuthenticator testling;

			testling.setCredentials("user", "pass", "authz");

			CPPUNIT_ASSERT_EQUAL(testling.getResponse(), ByteArray("authz\0user\0pass", 15));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PLAINClientAuthenticatorTest);
