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
#include <Swiften/SASL/DIGESTMD5ClientAuthenticator.h>

using namespace Swift;

class DIGESTMD5ClientAuthenticatorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(DIGESTMD5ClientAuthenticatorTest);
        CPPUNIT_TEST(testGetInitialResponse);
        CPPUNIT_TEST(testGetResponse);
        CPPUNIT_TEST(testGetResponse_WithAuthorizationID);
        //CPPUNIT_TEST(testSetChallenge);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            crypto = std::shared_ptr<CryptoProvider>(PlatformCryptoProvider::create());
        }

        void testGetInitialResponse() {
            DIGESTMD5ClientAuthenticator testling("xmpp.example.com", "abcdefgh", crypto.get());

            CPPUNIT_ASSERT(!testling.getResponse());
        }

        void testGetResponse() {
            DIGESTMD5ClientAuthenticator testling("xmpp.example.com", "abcdefgh", crypto.get());

            testling.setCredentials("user", createSafeByteArray("pass"), "");
            testling.setChallenge(createByteArray(
                "realm=\"example.com\","
                "nonce=\"O6skKPuaCZEny3hteI19qXMBXSadoWs840MchORo\","
                "qop=auth,charset=utf-8,algorithm=md5-sess"));

            SafeByteArray response = *testling.getResponse();

            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("charset=utf-8,cnonce=\"abcdefgh\",digest-uri=\"xmpp/xmpp.example.com\",nc=00000001,nonce=\"O6skKPuaCZEny3hteI19qXMBXSadoWs840MchORo\",qop=auth,realm=\"example.com\",response=088891c800ecff1b842159ad6459104a,username=\"user\""), response);
        }

        void testGetResponse_WithAuthorizationID() {
            DIGESTMD5ClientAuthenticator testling("xmpp.example.com", "abcdefgh", crypto.get());

            testling.setCredentials("user", createSafeByteArray("pass"), "myauthzid");
            testling.setChallenge(createByteArray(
                "realm=\"example.com\","
                "nonce=\"O6skKPuaCZEny3hteI19qXMBXSadoWs840MchORo\","
                "qop=auth,charset=utf-8,algorithm=md5-sess"));

            SafeByteArray response = *testling.getResponse();

            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("authzid=\"myauthzid\",charset=utf-8,cnonce=\"abcdefgh\",digest-uri=\"xmpp/xmpp.example.com\",nc=00000001,nonce=\"O6skKPuaCZEny3hteI19qXMBXSadoWs840MchORo\",qop=auth,realm=\"example.com\",response=4293834432b6e7889a2dee7e8fe7dd06,username=\"user\""), response);
        }

    private:
        std::shared_ptr<CryptoProvider> crypto;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DIGESTMD5ClientAuthenticatorTest);
