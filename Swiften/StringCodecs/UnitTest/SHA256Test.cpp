/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/SHA256.h>

using namespace Swift;

class SHA256Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(SHA256Test);
		CPPUNIT_TEST(testGetHashStatic_Empty);
		CPPUNIT_TEST(testGetHashStatic_Small);
		CPPUNIT_TEST(testGetHashStatic_Large);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetHashStatic_Empty() {
			ByteArray result(SHA256::getHash(createByteArray("")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\xe3\xb0\xc4" "B" "\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99" "o" "\xb9" "$'" "\xae" "A" "\xe4" "d" "\x9b\x93" "L" "\xa4\x95\x99\x1b" "xR" "\xb8" "U", 32), result);
		}

		void testGetHashStatic_Small() {
			ByteArray result(SHA256::getHash(createByteArray("abc")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\xba\x78\x16\xbf\x8f\x01\xcf\xea\x41\x41\x40\xde\x5d\xae\x22\x23\xb0\x03\x61\xa3\x96\x17\x7a\x9c\xb4\x10\xff\x61\xf2\x00\x15\xad", 32), result);
		}

		void testGetHashStatic_Large() {
			ByteArray result(SHA256::getHash(createByteArray("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\x24\x8d\x6a\x61\xd2\x06\x38\xb8\xe5\xc0\x26\x93\x0c\x3e\x60\x39\xa3\x3c\xe4\x59\x64\xff\x21\x67\xf6\xec\xed\xd4\x19\xdb\x06\xc1", 32), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SHA256Test);
