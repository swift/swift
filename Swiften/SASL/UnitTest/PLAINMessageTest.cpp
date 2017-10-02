/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/SASL/PLAINMessage.h>

using namespace Swift;

class PLAINMessageTest : public CppUnit::TestFixture
{
        CPPUNIT_TEST_SUITE(PLAINMessageTest);
        CPPUNIT_TEST(testGetValue_WithoutAuthzID);
        CPPUNIT_TEST(testGetValue_WithAuthzID);
        CPPUNIT_TEST(testConstructor_WithoutAuthzID);
        CPPUNIT_TEST(testConstructor_WithAuthzID);
        CPPUNIT_TEST(testConstructor_NoAuthcid);
        CPPUNIT_TEST(testConstructor_NoPassword);
        CPPUNIT_TEST_SUITE_END();

    public:
        PLAINMessageTest() {}

        void testGetValue_WithoutAuthzID() {
            PLAINMessage message("user", createSafeByteArray("pass"));
            CPPUNIT_ASSERT_EQUAL(message.getValue(), createSafeByteArray("\0user\0pass", 10));
        }

        void testGetValue_WithAuthzID() {
            PLAINMessage message("user", createSafeByteArray("pass"), "authz");
            CPPUNIT_ASSERT_EQUAL(message.getValue(), createSafeByteArray("authz\0user\0pass", 15));
        }

        void testConstructor_WithoutAuthzID() {
            PLAINMessage message(createSafeByteArray("\0user\0pass", 10));

            CPPUNIT_ASSERT_EQUAL(std::string(""), message.getAuthorizationID());
            CPPUNIT_ASSERT_EQUAL(std::string("user"), message.getAuthenticationID());
            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("pass"), message.getPassword());
        }

        void testConstructor_WithAuthzID() {
            PLAINMessage message(createSafeByteArray("authz\0user\0pass", 15));

            CPPUNIT_ASSERT_EQUAL(std::string("authz"), message.getAuthorizationID());
            CPPUNIT_ASSERT_EQUAL(std::string("user"), message.getAuthenticationID());
            CPPUNIT_ASSERT_EQUAL(createSafeByteArray("pass"), message.getPassword());
        }

        void testConstructor_NoAuthcid() {
            PLAINMessage message(createSafeByteArray("authzid", 7));

            CPPUNIT_ASSERT_EQUAL(std::string(""), message.getAuthenticationID());
        }

        void testConstructor_NoPassword() {
            PLAINMessage message(createSafeByteArray("authzid\0authcid", 15));

            CPPUNIT_ASSERT_EQUAL(std::string(""), message.getAuthenticationID());
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PLAINMessageTest);
