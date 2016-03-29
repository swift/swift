/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/lexical_cast.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>

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

			CPPUNIT_ASSERT_EQUAL(reinterpret_cast<const char*>(NULL), reinterpret_cast<const char*>(vecptr(testling)));
		}

		void testToString() {
			ByteArray testling(createByteArray("abcde"));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), byteArrayToString(testling));
		}

		void testToString_NullTerminated() {
			ByteArray testling(createByteArray("abcde\0", 6));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), byteArrayToString(testling));
		}

		void testToString_TwoNullTerminated() {
			ByteArray testling(createByteArray("abcde\0\0", 7));

			CPPUNIT_ASSERT_EQUAL(std::string("abcde"), byteArrayToString(testling));
		}

		void testToString_AllNull() {
			ByteArray testling(createByteArray("\0\0", 2));

			CPPUNIT_ASSERT_EQUAL(std::string(""), byteArrayToString(testling));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(ByteArrayTest);
