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
#include <Swiften/StringCodecs/PBKDF2.h>
#include <Swiften/StringCodecs/HMAC_SHA1.h>

using namespace Swift;

class PBKDF2Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PBKDF2Test);
		CPPUNIT_TEST(testGetResult_I1);
		CPPUNIT_TEST(testGetResult_I2);
		CPPUNIT_TEST(testGetResult_I4096);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testGetResult_I1() {
			ByteArray result(PBKDF2::encode<HMAC_SHA1 >(createSafeByteArray("password"), createByteArray("salt"), 1));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x0c\x60\xc8\x0f\x96\x1f\x0e\x71\xf3\xa9\xb5\x24\xaf\x60\x12\x06\x2f\xe0\x37\xa6"), result);
		}

		void testGetResult_I2() {
			ByteArray result(PBKDF2::encode<HMAC_SHA1 >(createSafeByteArray("password"), createByteArray("salt"), 2));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\xea\x6c\x1\x4d\xc7\x2d\x6f\x8c\xcd\x1e\xd9\x2a\xce\x1d\x41\xf0\xd8\xde\x89\x57"), result);
		}

		void testGetResult_I4096() {
			ByteArray result(PBKDF2::encode<HMAC_SHA1 >(createSafeByteArray("password"), createByteArray("salt"), 4096));

			CPPUNIT_ASSERT_EQUAL(createByteArray("\x4b\x00\x79\x1\xb7\x65\x48\x9a\xbe\xad\x49\xd9\x26\xf7\x21\xd0\x65\xa4\x29\xc1", 20), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(PBKDF2Test);
