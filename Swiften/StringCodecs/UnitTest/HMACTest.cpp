/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/StringCodecs/HMAC_SHA1.h>
#include <Swiften/StringCodecs/HMAC_SHA256.h>

using namespace Swift;

class HMACTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HMACTest);
		CPPUNIT_TEST(testGetResult);
		CPPUNIT_TEST(testGetResult_KeyLongerThanBlockSize);
		CPPUNIT_TEST(testGetResult_RFC4231_1);
		CPPUNIT_TEST(testGetResult_RFC4231_7);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetResult() {
			ByteArray result(HMAC_SHA1()(createSafeByteArray("foo"), createByteArray("foobar")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\xa4\xee\xba\x8e\x63\x3d\x77\x88\x69\xf5\x68\xd0\x5a\x1b\x3d\xc7\x2b\xfd\x4\xdd"), result);
		}

		void testGetResult_KeyLongerThanBlockSize() {
			ByteArray result(HMAC_SHA1()(createSafeByteArray("---------|---------|---------|---------|---------|----------|---------|"), createByteArray("foobar")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\xd6""n""\x8f""P|1""\xd3"",""\x6"" ""\xb9\xe3""gg""\x8e\xcf"" ]+""\xa"), result);
		}

		void testGetResult_RFC4231_1() {
			ByteArray result(HMAC_SHA256()(createSafeByteArray("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b", 20), createByteArray("Hi There")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1\x2b\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32\xcf\xf7", 32), result);
		}

		void testGetResult_RFC4231_7() {
			ByteArray result(HMAC_SHA256()(createSafeByteArray("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa", 131), createByteArray("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.")));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\x9b\x09\xff\xa7\x1b\x94\x2f\xcb\x27\x63\x5f\xbc\xd5\xb0\xe9\x44\xbf\xdc\x63\x64\x4f\x07\x13\x93\x8a\x7f\x51\x53\x5c\x3a\x35\xe2", 32), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(HMACTest);
