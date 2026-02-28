/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Platform.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#ifdef SWIFTEN_PLATFORM_WIN32
#include <Swiften/Crypto/WindowsCryptoProvider.h>
#endif
#ifdef HAVE_OPENSSL_CRYPTO_PROVIDER
#include <Swiften/Crypto/OpenSSLCryptoProvider.h>
#endif
#ifdef HAVE_COMMONCRYPTO_CRYPTO_PROVIDER
#include <Swiften/Crypto/CommonCryptoCryptoProvider.h>
#endif
#include <Swiften/Crypto/Hash.h>

using namespace Swift;

template <typename CryptoProviderType>
class CryptoProviderTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CryptoProviderTest);

        CPPUNIT_TEST(testGetSHA1Hash);
        CPPUNIT_TEST(testGetSHA1Hash_TwoUpdates);
        CPPUNIT_TEST(testGetSHA1Hash_NoData);
        CPPUNIT_TEST(testGetSHA1HashStatic);
        CPPUNIT_TEST(testGetSHA1HashStatic_Twice);
        CPPUNIT_TEST(testGetSHA1HashStatic_NoData);

        CPPUNIT_TEST(testGetMD5Hash_Empty);
        CPPUNIT_TEST(testGetMD5Hash_Alphabet);
        CPPUNIT_TEST(testMD5Incremental);

        CPPUNIT_TEST(testGetHMACSHA1);
        CPPUNIT_TEST(testGetHMACSHA1_KeyLongerThanBlockSize);

        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            provider = new CryptoProviderType();
        }

        void tearDown() {
            delete provider;
        }

        ////////////////////////////////////////////////////////////
        // SHA-1
        ////////////////////////////////////////////////////////////

        void testGetSHA1Hash() {
            std::shared_ptr<Hash> sha = std::shared_ptr<Hash>(provider->createSHA1());
            sha->update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha->getHash());
        }

        void testGetSHA1Hash_TwoUpdates() {
            std::shared_ptr<Hash> sha = std::shared_ptr<Hash>(provider->createSHA1());
            sha->update(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<"));
            sha->update(createByteArray("http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), sha->getHash());
        }

        void testGetSHA1Hash_NoData() {
            std::shared_ptr<Hash> sha = std::shared_ptr<Hash>(provider->createSHA1());
            sha->update(std::vector<unsigned char>());

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"), sha->getHash());
        }

        void testGetSHA1HashStatic() {
            ByteArray result(provider->getSHA1Hash(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<")));
            CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
        }


        void testGetSHA1HashStatic_Twice() {
            ByteArray input(createByteArray("client/pc//Exodus 0.9.1<http://jabber.org/protocol/caps<http://jabber.org/protocol/disco#info<http://jabber.org/protocol/disco#items<http://jabber.org/protocol/muc<"));
            provider->getSHA1Hash(input);
            ByteArray result(provider->getSHA1Hash(input));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\x42\x06\xb2\x3c\xa6\xb0\xa6\x43\xd2\x0d\x89\xb0\x4f\xf5\x8c\xf7\x8b\x80\x96\xed"), result);
        }

        void testGetSHA1HashStatic_NoData() {
            ByteArray result(provider->getSHA1Hash(ByteArray()));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09"), result);
        }


        ////////////////////////////////////////////////////////////
        // MD5
        ////////////////////////////////////////////////////////////

        void testGetMD5Hash_Empty() {
            ByteArray result(provider->getMD5Hash(createByteArray("")));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e", 16), result);
        }

        void testGetMD5Hash_Alphabet() {
            ByteArray result(provider->getMD5Hash(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")));

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f", 16), result);
        }

        void testMD5Incremental() {
            std::shared_ptr<Hash> testling = std::shared_ptr<Hash>(provider->createMD5());
            testling->update(createByteArray("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
            testling->update(createByteArray("abcdefghijklmnopqrstuvwxyz0123456789"));

            ByteArray result = testling->getHash();

            CPPUNIT_ASSERT_EQUAL(createByteArray("\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f", 16), result);
        }


        ////////////////////////////////////////////////////////////
        // HMAC-SHA1
        ////////////////////////////////////////////////////////////

        void testGetHMACSHA1() {
            ByteArray result(provider->getHMACSHA1(createSafeByteArray("foo"), createByteArray("foobar")));
            CPPUNIT_ASSERT_EQUAL(createByteArray("\xa4\xee\xba\x8e\x63\x3d\x77\x88\x69\xf5\x68\xd0\x5a\x1b\x3d\xc7\x2b\xfd\x4\xdd"), result);
        }

        void testGetHMACSHA1_KeyLongerThanBlockSize() {
            ByteArray result(provider->getHMACSHA1(createSafeByteArray("---------|---------|---------|---------|---------|----------|---------|"), createByteArray("foobar")));
            CPPUNIT_ASSERT_EQUAL(createByteArray("\xd6""n""\x8f""P|1""\xd3"",""\x6"" ""\xb9\xe3""gg""\x8e\xcf"" ]+""\xa"), result);
        }

    private:
        CryptoProviderType* provider;
};

#ifdef SWIFTEN_PLATFORM_WIN32
CPPUNIT_TEST_SUITE_REGISTRATION(CryptoProviderTest<WindowsCryptoProvider>);
#endif
#ifdef HAVE_OPENSSL_CRYPTO_PROVIDER
CPPUNIT_TEST_SUITE_REGISTRATION(CryptoProviderTest<OpenSSLCryptoProvider>);
#endif
#ifdef HAVE_COMMONCRYPTO_CRYPTO_PROVIDER
CPPUNIT_TEST_SUITE_REGISTRATION(CryptoProviderTest<CommonCryptoCryptoProvider>);
#endif
