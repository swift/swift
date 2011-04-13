/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/ByteArray.h"
#include <boost/lexical_cast.hpp>

using namespace Swift;

class ByteArrayTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ByteArrayTest);
		CPPUNIT_TEST(testGetData_NoData);
		CPPUNIT_TEST(testToString);
		CPPUNIT_TEST(testToString_NullTerminated);
		CPPUNIT_TEST(testToString_TwoNullTerminated);
		CPPUNIT_TEST(testToString_AllNull);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetData_NoData() {
			ByteArray testling;

			CPPUNIT_ASSERT_EQUAL(reinterpret_cast<const char*>(NULL), reinterpret_cast<const char*>(testling.getData()));
		}

		void testToString() {
			ByteArray testling(ByteArray::create("abcde"));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), testling.toString());
		}

		void testToString_NullTerminated() {
			ByteArray testling(ByteArray::create("abcde\0", 6));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), testling.toString());
		}

		void testToString_TwoNullTerminated() {
			ByteArray testling(ByteArray::create("abcde\0\0", 7));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), testling.toString());
		}

		void testToString_AllNull() {
			ByteArray testling(ByteArray::create("\0\0", 2));

			CPPUNIT_ASSERT_EQUAL(std::string(""), testling.toString());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ByteArrayTest);
