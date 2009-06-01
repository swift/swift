#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/SASL/PLAINMessage.h"

using namespace Swift;

class PLAINMessageTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(PLAINMessageTest);
		CPPUNIT_TEST(testConstructor_WithoutAuthzID);
		CPPUNIT_TEST(testConstructor_WithAuthzID);
		CPPUNIT_TEST_SUITE_END();

	public:
		PLAINMessageTest() {}

		void testConstructor_WithoutAuthzID() {
			PLAINMessage message("user", "pass");
			CPPUNIT_ASSERT_EQUAL(message.getValue(), ByteArray("\0user\0pass", 10));
		}

		void testConstructor_WithAuthzID() {
			PLAINMessage message("user", "pass", "authz");
			CPPUNIT_ASSERT_EQUAL(message.getValue(), ByteArray("authz\0user\0pass", 15));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PLAINMessageTest);
