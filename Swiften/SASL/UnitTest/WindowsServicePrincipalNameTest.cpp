/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>

#include <Swiften/SASL/WindowsServicePrincipalName.h>

using namespace Swift;

class WindowsServicePrincipalNameTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(WindowsServicePrincipalNameTest);
        CPPUNIT_TEST(testServiceClass);
        CPPUNIT_TEST(testServiceName);
        CPPUNIT_TEST(testInstanceName);
        CPPUNIT_TEST(testInstancePort);
        CPPUNIT_TEST(testReferrer);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testServiceClass() {
            WindowsServicePrincipalName spn("mlink.adlon.isode.net");

            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/mlink.adlon.isode.net"));

            spn.setServiceClass("ldap");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("ldap/mlink.adlon.isode.net"));

            spn.setServiceClass("中文");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("中文/mlink.adlon.isode.net"));

            try {
                spn.setServiceClass("");
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }

            try {
                spn.setServiceClass("xm/pp"); /* Foward slash not allowed */
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }
        }

        void testServiceName() {
            try {
                WindowsServicePrincipalName spn("");
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }

            try {
                WindowsServicePrincipalName spn2("mlink/adlon.isode.net"); /* Foward slash not allowed */
                spn2.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }

            WindowsServicePrincipalName spn3("mlinkÄ.adlon.isode.net");
            CPPUNIT_ASSERT_EQUAL(spn3.toString(), std::string("xmpp/mlinkÄ.adlon.isode.net"));
        }

        void testInstanceName() {
            WindowsServicePrincipalName spn("adlon.isode.net");

            spn.setInstanceName("mlink.adlon.isode.net");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/mlink.adlon.isode.net/adlon.isode.net"));

            spn.setInstanceName("127.0.0.1");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/127.0.0.1/adlon.isode.net"));

            spn.setInstanceName("");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/adlon.isode.net"));

            spn.setInstanceName("cañón.adlon.isode.net");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/cañón.adlon.isode.net/adlon.isode.net"));

            try {
                spn.setInstanceName("mlink/adlon.isode.net"); /* Foward slash not allowed */
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }
        }

        void testInstancePort() {
            WindowsServicePrincipalName spn("adlon.isode.net");

            spn.setInstanceName("mlink.adlon.isode.net");
            spn.setInstancePort(55222);
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/mlink.adlon.isode.net:55222/adlon.isode.net"));

            spn.setInstancePort(0);
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/mlink.adlon.isode.net/adlon.isode.net"));

            WindowsServicePrincipalName spn2("mlink.adlon.isode.net");

            spn2.setInstancePort(55222);
            CPPUNIT_ASSERT_EQUAL(spn2.toString(), std::string("xmpp/mlink.adlon.isode.net:55222"));

            spn2.setInstancePort(0);
            CPPUNIT_ASSERT_EQUAL(spn2.toString(), std::string("xmpp/mlink.adlon.isode.net"));
        }

        void testReferrer() {
            WindowsServicePrincipalName spn("127.0.0.1");

            spn.setReferrer("referrer.net");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/127.0.0.1/referrer.net"));

            spn.setInstancePort(6222);
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/127.0.0.1:6222/referrer.net"));

            spn.setReferrer("हिन्दी.net");
            CPPUNIT_ASSERT_EQUAL(spn.toString(), std::string("xmpp/127.0.0.1:6222/हिन्दी.net"));

            try {
                spn.setReferrer("referrer/net"); /* Foward slash not allowed */
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }

            try {
                spn.setReferrer(""); /* seems like you must have referrer with an IP */
                spn.toString();
                CPPUNIT_ASSERT(false);
            } catch (std::runtime_error) {
                /* expected */
            }

            WindowsServicePrincipalName spn2("mlink.adlon.isode.net");

            spn2.setReferrer("referrer.net"); /* Referrer ignored if service name is not IP */
            CPPUNIT_ASSERT_EQUAL(spn2.toString(), std::string("xmpp/mlink.adlon.isode.net"));

            spn2.setReferrer("referrer/net"); /* Referrer ignored if service name is not IP */
            CPPUNIT_ASSERT_EQUAL(spn2.toString(), std::string("xmpp/mlink.adlon.isode.net"));

            WindowsServicePrincipalName spn3("adlon.isode.net");

            spn3.setInstanceName("mlink.adlon.isode.net");
            spn3.setInstancePort(6222);
            spn3.setReferrer("referrer.net"); /* Referrer ignored if service name is not IP */
            CPPUNIT_ASSERT_EQUAL(spn3.toString(), std::string("xmpp/mlink.adlon.isode.net:6222/adlon.isode.net"));

        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WindowsServicePrincipalNameTest);
