/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/MD5.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

class MD5Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(MD5Test);
		CPPUNIT_TEST(testGetHash_Empty);
		CPPUNIT_TEST(testGetHash_Alphabet);
		CPPUNIT_TEST(testIncrementalTest);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetHash_Empty() {
			ByteArray result(MD5::getHash(createByteArray("")));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e", 16), result);
		}

		void testGetHash_Alphabet() {
			ByteArray result(MD5::getHash(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f", 16), result);
		}

		void testIncrementalTest() {
			MD5 testling;
			testling.update(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			testling.update(createByteArray("abcdefghijklmnopqrstuvwxyz0123456789"));

			ByteArray result = testling.getHash();

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f", 16), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(MD5Test);
