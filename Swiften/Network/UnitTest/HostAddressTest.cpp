#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Network/HostAddress.h"

using namespace Swift;

class HostAddressTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(HostAddressTest);
		CPPUNIT_TEST(testToString);
		CPPUNIT_TEST(testToString_IPv6);
		CPPUNIT_TEST_SUITE_END();

	public:
		HostAddressTest() {}

		void testToString() {
			unsigned char address[4] = {10, 0, 1, 253}; 
			HostAddress testling(address, 4);

			CPPUNIT_ASSERT_EQUAL(std::string("10.0.1.253"), testling.toString());
		}

		void testToString_IPv6() {
			unsigned char address[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}; 
			HostAddress testling(address, 16);

			CPPUNIT_ASSERT_EQUAL(std::string("0102:0304:0506:0708:090a:0b0c:0d0e:0f11"), testling.toString());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(HostAddressTest);
