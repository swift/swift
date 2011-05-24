/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

class Base64Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(Base64Test);
		CPPUNIT_TEST(testEncode);
		CPPUNIT_TEST(testEncode_NonAscii);
		CPPUNIT_TEST(testEncode_NoData);
		CPPUNIT_TEST(testDecode);
		CPPUNIT_TEST(testDecode_NoData);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testEncode() {
			std::string result(Base64::encode(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890")));
			CPPUNIT_ASSERT_EQUAL(std::string("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejEyMzQ1Njc4OTA="), result);
		}

		void testEncode_NonAscii() {
			std::string result(Base64::encode(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed")));
			CPPUNIT_ASSERT_EQUAL(std::string("QgayPKawpkPSDYmwT/WM94uAlu0="), result);
		}

		void testEncode_NoData() {
			std::string result(Base64::encode(ByteArray()));
			CPPUNIT_ASSERT_EQUAL(std::string(""), result);
		}

		void testDecode() {
			ByteArray result(Base64::decode("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejEyMzQ1Njc4OTA="));
			CPPUNIT_ASSERT_EQUAL(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"), result);
		}

		void testDecode_NoData() {
			ByteArray result(Base64::decode(""));
			CPPUNIT_ASSERT_EQUAL(ByteArray(), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Base64Test);
