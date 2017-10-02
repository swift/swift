/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/SASL/EXTERNALClientAuthenticator.h>

using namespace Swift;

class EXTERNALClientAuthenticatorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(EXTERNALClientAuthenticatorTest);
        CPPUNIT_TEST(testGetResponse_WithoutAuthzID);
        CPPUNIT_TEST(testGetResponse_WithAuthzID);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testGetResponse_WithoutAuthzID() {
            EXTERNALClientAuthenticator testling;

            // Authcid and password are not used (ignored)
            testling.setCredentials("user", createSafeByteArray("pass"));

            boost::optional<SafeByteArray> response = testling.getResponse();

            // No data should have been returned
            bool result = !response;

            CPPUNIT_ASSERT(result);
        }

        void testGetResponse_WithAuthzID() {
            EXTERNALClientAuthenticator testling;

            // Authcid and password are not used (ignored)
            testling.setCredentials("user", createSafeByteArray("pass"), "authz");

            CPPUNIT_ASSERT_EQUAL(*testling.getResponse(), createSafeByteArray("authz", 5));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(EXTERNALClientAuthenticatorTest);
