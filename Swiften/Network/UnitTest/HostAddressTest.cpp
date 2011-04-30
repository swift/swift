/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Network/HostAddress.h>
#include <string>

using namespace Swift;

class HostAddressTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HostAddressTest);
		CPPUNIT_TEST(testConstructor);
		CPPUNIT_TEST(testConstructor_Invalid);
		CPPUNIT_TEST(testConstructor_InvalidString);
		CPPUNIT_TEST(testToString);
		CPPUNIT_TEST(testToString_IPv6);
		CPPUNIT_TEST(testToString_Invalid);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testConstructor() {
			HostAddress testling("192.168.1.254");

			CPPUNIT_ASSERT_EQUAL(std::string("192.168.1.254"), testling.toString());
			CPPUNIT_ASSERT(testling.isValid());
		}

		void testConstructor_Invalid() {
			HostAddress testling;

			CPPUNIT_ASSERT(!testling.isValid());
		}

		void testConstructor_InvalidString() {
			HostAddress testling("invalid");

			CPPUNIT_ASSERT(!testling.isValid());
		}

		void testToString() {
			unsigned char address[4] = {10, 0, 1, 253}; 
			HostAddress testling(address, 4);

			CPPUNIT_ASSERT_EQUAL(std::string("10.0.1.253"), testling.toString());
		}

		void testToString_IPv6() {
			unsigned char address[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17}; 
			HostAddress testling(address, 16);

			CPPUNIT_ASSERT_EQUAL(std::string("102:304:506:708:90a:b0c:d0e:f11"), testling.toString());
		}

		void testToString_Invalid() {
			HostAddress testling;

			CPPUNIT_ASSERT_EQUAL(std::string("0.0.0.0"), testling.toString());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(HostAddressTest);
