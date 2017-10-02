/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/Platform.h>
#include <Swiften/Base/String.h>

#include <boost/format.hpp>

using namespace Swift;

 class StringTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(StringTest);
        CPPUNIT_TEST(testGetUnicodeCodePoints);
        CPPUNIT_TEST(testGetSplittedAtFirst);
        CPPUNIT_TEST(testGetSplittedAtFirst_CharacterAtBegin);
        CPPUNIT_TEST(testGetSplittedAtFirst_CharacterAtEnd);
        CPPUNIT_TEST(testGetSplittedAtFirst_NoSuchCharacter);
        CPPUNIT_TEST(testReplaceAll);
        CPPUNIT_TEST(testReplaceAll_LastChar);
        CPPUNIT_TEST(testReplaceAll_ConsecutiveChars);
        CPPUNIT_TEST(testReplaceAll_MatchingReplace);
        CPPUNIT_TEST(testIsValidXMPPCharacter);
        CPPUNIT_TEST(testSanitizeXMPPString);
        CPPUNIT_TEST(testSplit);
#ifdef SWIFTEN_PLATFORM_WINDOWS
        CPPUNIT_TEST(testConvertWStringToString);
        CPPUNIT_TEST(testConvertStringToWString);
#endif
        CPPUNIT_TEST_SUITE_END();

    public:
        void testGetUnicodeCodePoints() {
            std::string testling("$\xc2\xa2\xe2\x82\xac\xf4\x8a\xaf\x8d");
            std::vector<unsigned int> points = String::getUnicodeCodePoints(testling);

            CPPUNIT_ASSERT_EQUAL(0x24U, points[0]);
            CPPUNIT_ASSERT_EQUAL(0xA2U, points[1]);
            CPPUNIT_ASSERT_EQUAL(0x20ACU, points[2]);
            CPPUNIT_ASSERT_EQUAL(0x10ABCDU, points[3]);
        }

        void testGetSplittedAtFirst() {
            std::string testling("ab@cd@ef");

            std::pair<std::string,std::string> result = String::getSplittedAtFirst(testling, '@');
            CPPUNIT_ASSERT_EQUAL(std::string("ab"), result.first);
            CPPUNIT_ASSERT_EQUAL(std::string("cd@ef"), result.second);
        }

        void testGetSplittedAtFirst_CharacterAtBegin() {
            std::string testling(" ab");

            std::pair<std::string,std::string> result = String::getSplittedAtFirst(testling, ' ');
            CPPUNIT_ASSERT(result.first.empty());
            CPPUNIT_ASSERT_EQUAL(std::string("ab"), result.second);
        }

        void testGetSplittedAtFirst_CharacterAtEnd() {
            std::string testling("ab@");

            std::pair<std::string,std::string> result = String::getSplittedAtFirst(testling, '@');
            CPPUNIT_ASSERT_EQUAL(std::string("ab"), result.first);
            CPPUNIT_ASSERT(result.second.empty());
        }

        void testGetSplittedAtFirst_NoSuchCharacter() {
            std::string testling("ab");

            std::pair<std::string,std::string> result = String::getSplittedAtFirst(testling, '@');
            CPPUNIT_ASSERT_EQUAL(std::string("ab"), result.first);
            CPPUNIT_ASSERT(result.second.empty());
        }

        void testReplaceAll() {
            std::string testling("abcbd");

            String::replaceAll(testling, 'b', "xyz");

            CPPUNIT_ASSERT_EQUAL(std::string("axyzcxyzd"), testling);
        }

        void testReplaceAll_LastChar() {
            std::string testling("abc");

            String::replaceAll(testling, 'c', "xyz");

            CPPUNIT_ASSERT_EQUAL(std::string("abxyz"), testling);
        }

        void testReplaceAll_ConsecutiveChars() {
            std::string testling("abbc");

            String::replaceAll(testling, 'b',"xyz");

            CPPUNIT_ASSERT_EQUAL(std::string("axyzxyzc"), testling);
        }

        void testReplaceAll_MatchingReplace() {
            std::string testling("abc");

            String::replaceAll(testling, 'b',"bbb");

            CPPUNIT_ASSERT_EQUAL(std::string("abbbc"), testling);
        }

        void testIsValidXMPPCharacter() {
            const std::uint32_t testCharacters[] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                0x20, 0x7F, 0x80, 0x84, 0x85, 0xFF };
            const auto testLength = sizeof(testCharacters) / sizeof(std::uint32_t);
            const bool expectedValid[] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 1, 1 };
            static_assert(testLength == sizeof(expectedValid), "size of test data must match");

            for (std::size_t i = 0; i != testLength; ++i) {
                const auto c = testCharacters[i];
                CPPUNIT_ASSERT_EQUAL_MESSAGE(boost::str(boost::format("While testing at idx=%d: 0x%02x") % i % c), expectedValid[i], String::isValidXMPPCharacter(c));
            }
        }

        void testSanitizeXMPPString() {
            std::vector<std::pair<std::string, std::string>> testData = {
                { "\0", "" },
                { std::string("\0\t", 3), "\t" },
                { "", "" },
                { std::string("\0", 1) , std::string() },
                { std::string("\0blah\0", 6) , std::string("blah", 4) },
                { "z\xC3\x9F\xE6\xB0\xB4\xF0\x9D\x84\x8B" , "z\xC3\x9F\xE6\xB0\xB4\xF0\x9D\x84\x8B" }, // or in u8 notation: u8"z\u00df\u6c34\U0001d10b"
                { "\x7FT\t\x0c\xff\xfeT", "T\tT" },
                { "\x01Q\x0BW\x81T", "QWT" },
                { "\xF0\x9F\x98\x83" "ABC" "\xE2\xBE\xA6", "\xF0\x9F\x98\x83" "ABC" "\xE2\xBE\xA6" }
            };

            for (std::size_t i = 0; i != testData.size(); ++i) {
                const auto & t = testData[i];
                const auto actual = String::sanitizeXMPPString(t.first);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(boost::str(boost::format("While testing string idx=%d") % i), std::string(actual), t.second);
            }
        }

        void testSplit() {
            std::vector<std::string> result = String::split("abc def ghi", ' ');

            CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(result.size()));
            CPPUNIT_ASSERT_EQUAL(std::string("abc"), result[0]);
            CPPUNIT_ASSERT_EQUAL(std::string("def"), result[1]);
            CPPUNIT_ASSERT_EQUAL(std::string("ghi"), result[2]);
        }

#ifdef SWIFTEN_PLATFORM_WINDOWS
        void testConvertWStringToString() {
            CPPUNIT_ASSERT_EQUAL(std::string("tron\xc3\xa7on"), convertWStringToString(std::wstring(L"tron\xe7on")));
        }

        void testConvertStringToWString() {
            CPPUNIT_ASSERT(std::wstring(L"tron\xe7on") == convertStringToWString(std::string("tron\xc3\xa7on")));
        }
#endif
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);
