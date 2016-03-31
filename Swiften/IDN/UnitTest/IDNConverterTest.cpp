/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/shared_ptr.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>

using namespace Swift;

class IDNConverterTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(IDNConverterTest);
        CPPUNIT_TEST(testStringPrep);
        CPPUNIT_TEST(testStringPrep_Empty);
        CPPUNIT_TEST(testGetEncoded);
        CPPUNIT_TEST(testGetEncoded_International);
        CPPUNIT_TEST(testGetEncoded_Invalid);
        CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            testling = boost::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
        }

        void testStringPrep() {
            std::string result = testling->getStringPrepared("tron\xc3\x87on", IDNConverter::NamePrep);

            CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), result);
        }

        void testStringPrep_Empty() {
            CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::NamePrep));
            CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::XMPPNodePrep));
            CPPUNIT_ASSERT_EQUAL(std::string(""), testling->getStringPrepared("", IDNConverter::XMPPResourcePrep));
        }

        void testGetEncoded() {
            boost::optional<std::string> result = testling->getIDNAEncoded("www.swift.im");
            CPPUNIT_ASSERT(!!result);
            CPPUNIT_ASSERT_EQUAL(std::string("www.swift.im"), *result);
        }

        void testGetEncoded_International() {
            boost::optional<std::string> result = testling->getIDNAEncoded("www.tron\xc3\x87on.com");
            CPPUNIT_ASSERT(!!result);
            CPPUNIT_ASSERT_EQUAL(std::string("www.xn--tronon-zua.com"), *result);
        }

        void testGetEncoded_Invalid() {
            boost::optional<std::string> result = testling->getIDNAEncoded("www.foo,bar.com");
            CPPUNIT_ASSERT(!result);
        }

    private:
        boost::shared_ptr<IDNConverter> testling;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IDNConverterTest);
