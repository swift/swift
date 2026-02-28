/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/IDN/UTF8Validator.h>

using namespace Swift;

class UTF8ValidatorTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(UTF8ValidatorTest);

    CPPUNIT_TEST(testValidUTF8Sequences);
    CPPUNIT_TEST(testInvalidUTF8Sequences);

    CPPUNIT_TEST_SUITE_END();

public:
    void testValidUTF8Sequences() {
        {
            unsigned char test[] = {0x74, 0x65, 0x73, 0x74};
            CPPUNIT_ASSERT(UTF8IsValid(test, sizeof(test)));
        }

        {
            unsigned char test[] = {0xf4, 0x8f, 0x80, 0xbf};
            CPPUNIT_ASSERT(UTF8IsValid(test, sizeof(test)));
        }
    }

    void testInvalidUTF8Sequences() {
        {
            unsigned char test[] = {0x41, 0xC2, 0x3E, 0x42};
            CPPUNIT_ASSERT(!UTF8IsValid(test, sizeof(test)));
        }

        {
            unsigned char test[] = {0xf4};
            CPPUNIT_ASSERT(!UTF8IsValid(test, sizeof(test)));
        }

        {
            unsigned char test[] = {0xf4, 0x8f, 0x65, 0x73, 0x80, 0xbf};
            CPPUNIT_ASSERT(!UTF8IsValid(test, sizeof(test)));
        }
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(UTF8ValidatorTest);
