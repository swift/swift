/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/HostAddressPort.h>

using namespace Swift;

class HostAddressTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(HostAddressTest);
        CPPUNIT_TEST(testConstructor);
        CPPUNIT_TEST(testConstructor_Invalid);
        CPPUNIT_TEST(testConstructor_InvalidString);
        CPPUNIT_TEST(testToString);
        CPPUNIT_TEST(testToString_IPv6);
        CPPUNIT_TEST(testToString_Invalid);
        CPPUNIT_TEST(testComparison);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testConstructor() {
            auto testling = HostAddress::fromString("192.168.1.254");

            CPPUNIT_ASSERT_EQUAL(std::string("192.168.1.254"), testling->toString());
            CPPUNIT_ASSERT(testling->isValid());
        }

        void testConstructor_Invalid() {
            HostAddress testling;

            CPPUNIT_ASSERT(!testling.isValid());
        }

        void testConstructor_InvalidString() {
            auto testling = HostAddress::fromString("invalid");

            CPPUNIT_ASSERT(!testling);
        }

        void testToString() {
            unsigned char address[4] = {10, 0, 1, 253};
            HostAddress testling(address, 4);

            CPPUNIT_ASSERT_EQUAL(std::string("10.0.1.253"), testling.toString());
        }

        void testToString_IPv6() {
            unsigned char address[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17};
            HostAddress testling(address, 16);

            CPPUNIT_ASSERT_EQUAL(std::string("102:304:506:708:90a:b0c:d0e:f11"), testling.toString());
        }

        void testToString_Invalid() {
            HostAddress testling;

            CPPUNIT_ASSERT_EQUAL(std::string("0.0.0.0"), testling.toString());
        }

        void testComparison() {
            auto ha127_0_0_1 = *HostAddress::fromString("127.0.0.1");
            auto ha127_0_0_2 = *HostAddress::fromString("127.0.0.2");
            auto ha127_0_1_0 = *HostAddress::fromString("127.0.1.0");

            CPPUNIT_ASSERT(ha127_0_0_1 < ha127_0_0_2);
            CPPUNIT_ASSERT(ha127_0_0_2 < ha127_0_1_0);
            CPPUNIT_ASSERT(!(ha127_0_0_1 < ha127_0_0_1));
            CPPUNIT_ASSERT(!(ha127_0_0_2 < ha127_0_0_1));
            CPPUNIT_ASSERT(!(ha127_0_0_2 == ha127_0_0_1));
            CPPUNIT_ASSERT(ha127_0_0_1 == ha127_0_0_1);

            auto hap_127_0_0_1__1 = HostAddressPort(ha127_0_0_1, 1);
            auto hap_127_0_0_1__2 = HostAddressPort(ha127_0_0_1, 2);
            auto hap_127_0_0_2__1 = HostAddressPort(ha127_0_0_2, 1);
            auto hap_127_0_0_2__2 = HostAddressPort(ha127_0_0_2, 2);

            CPPUNIT_ASSERT(hap_127_0_0_1__1 < hap_127_0_0_1__2);
            CPPUNIT_ASSERT(!(hap_127_0_0_1__1 < hap_127_0_0_1__1));
            CPPUNIT_ASSERT(!(hap_127_0_0_1__1 == hap_127_0_0_1__2));
            CPPUNIT_ASSERT(hap_127_0_0_1__1 == hap_127_0_0_1__1);
            CPPUNIT_ASSERT(!(hap_127_0_0_1__2 == hap_127_0_0_1__1));
            CPPUNIT_ASSERT(hap_127_0_0_1__2 < hap_127_0_0_2__1);
            CPPUNIT_ASSERT(hap_127_0_0_2__1 < hap_127_0_0_2__2);
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HostAddressTest);
