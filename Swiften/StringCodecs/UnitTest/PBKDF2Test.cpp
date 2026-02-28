/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/StringCodecs/PBKDF2.h>

using namespace Swift;

class PBKDF2Test : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PBKDF2Test);
        CPPUNIT_TEST(testGetResult_I1);
        CPPUNIT_TEST(testGetResult_I2);
        CPPUNIT_TEST(testGetResult_I4096);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
        }

        void testGetResult_I1() {
            ByteArray result(PBKDF2::encode(createSafeByteArray("password"), createByteArray("salt"), 1, crypto.get()));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\x0c\x60\xc8\x0f\x96\x1f\x0e\x71\xf3\xa9\xb5\x24\xaf\x60\x12\x06\x2f\xe0\x37\xa6"), result);
        }

        void testGetResult_I2() {
            ByteArray result(PBKDF2::encode(createSafeByteArray("password"), createByteArray("salt"), 2, crypto.get()));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xea\x6c\x1\x4d\xc7\x2d\x6f\x8c\xcd\x1e\xd9\x2a\xce\x1d\x41\xf0\xd8\xde\x89\x57"), result);
        }

        void testGetResult_I4096() {
            ByteArray result(PBKDF2::encode(createSafeByteArray("password"), createByteArray("salt"), 4096, crypto.get()));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\x4b\x00\x79\x1\xb7\x65\x48\x9a\xbe\xad\x49\xd9\x26\xf7\x21\xd0\x65\xa4\x29\xc1", 20), result);
        }

    private:
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PBKDF2Test);
