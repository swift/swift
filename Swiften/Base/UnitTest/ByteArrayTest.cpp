#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/ByteArray.h"

using namespace Swift;

class ByteArrayTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ByteArrayTest);
		CPPUNIT_TEST(testGetData_NoData);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetData_NoData() {
			ByteArray testling;

			CPPUNIT_ASSERT_EQUAL(static_cast<const char*>(NULL), static_cast<const char*>(testling.getData()));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ByteArrayTest);
