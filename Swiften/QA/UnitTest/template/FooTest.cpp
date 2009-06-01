#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

using namespace Swift;

class FooTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(FooTest);
		CPPUNIT_TEST(testBar);
		CPPUNIT_TEST_SUITE_END();

	public:
		FooTest() {}

		void setUp() {
		}

		void tearDown() {
		}

		void testBar() {
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(FooTest);
