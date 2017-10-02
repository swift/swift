/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/SASL/PLAINClientAuthenticator.h>

using namespace Swift;

class PLAINClientAuthenticatorTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(PLAINClientAuthenticatorTest);
        CPPUNIT_TEST(testGetResponse_WithoutAuthzID);
        CPPUNIT_TEST(testGetResponse_WithAuthzID);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testGetResponse_WithoutAuthzID() {
            PLAINClientAuthenticator testling;

            testling.setCredentials("user", createSafeByteArray("pass"));

            CPPUNIT_ASSERT_EQUAL(*testling.getResponse(), createSafeByteArray("\0user\0pass", 10));
        }

        void testGetResponse_WithAuthzID() {
            PLAINClientAuthenticator testling;

            testling.setCredentials("user", createSafeByteArray("pass"), "authz");

            CPPUNIT_ASSERT_EQUAL(*testling.getResponse(), createSafeByteArray("authz\0user\0pass", 15));
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PLAINClientAuthenticatorTest);
