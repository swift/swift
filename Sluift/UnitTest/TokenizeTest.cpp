/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Sluift/tokenize.h>

using namespace Swift;

class TokenizeTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(TokenizeTest);
        CPPUNIT_TEST(testTokenize);
        CPPUNIT_TEST(testTokenize);
        CPPUNIT_TEST(testTokenize_String);
        CPPUNIT_TEST(testTokenize_IncompleteString);
        CPPUNIT_TEST(testTokenize_Identifier);
        CPPUNIT_TEST_SUITE_END();

    public:
        void testTokenize() {
            std::vector<std::string> tokens = Lua::tokenize("foo.bar + 1.23 - bam");

            CPPUNIT_ASSERT_EQUAL(7, static_cast<int>(tokens.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo"), tokens[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("."), tokens[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("bar"), tokens[2]);
            CPPUNIT_ASSERT_EQUAL(std::string("+"), tokens[3]);
            CPPUNIT_ASSERT_EQUAL(std::string("1.23"), tokens[4]);
            CPPUNIT_ASSERT_EQUAL(std::string("-"), tokens[5]);
            CPPUNIT_ASSERT_EQUAL(std::string("bam"), tokens[6]);
        }

        void testTokenize_String() {
            std::vector<std::string> tokens = Lua::tokenize("  foo   ..   \"1234\\\"bla blo\"");

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(tokens.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo"), tokens[0]);
            CPPUNIT_ASSERT_EQUAL(std::string(".."), tokens[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("\"1234\\\"bla blo\""), tokens[2]);
        }

        void testTokenize_IncompleteString() {
            std::vector<std::string> tokens = Lua::tokenize("\"1234");

            CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(tokens.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("\"1234"), tokens[0]);
        }

        void testTokenize_Identifier() {
            std::vector<std::string> tokens = Lua::tokenize("foo.bar_baz");

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(tokens.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("foo"), tokens[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("."), tokens[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("bar_baz"), tokens[2]);
        }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TokenizeTest);
