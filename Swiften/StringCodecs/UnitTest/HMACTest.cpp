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

using namespace Swift;

class HMACTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(HMACTest);
		CPPUNIT_TEST(testGetResult);
		CPPUNIT_TEST(testGetResult_KeyLongerThanBlockSize);
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(HMACTest);
