/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <string>

#include <Swiften/Base/String.h>

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
		CPPUNIT_TEST(testSplit);
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

		void testSplit() {
			std::vector<std::string> result = String::split("abc def ghi", ' ');

			CPPUNIT_ASSERT_EQUAL(3, static_cast<int>(result.size()));
			CPPUNIT_ASSERT_EQUAL(std::string("abc"), result[0]);
			CPPUNIT_ASSERT_EQUAL(std::string("def"), result[1]);
			CPPUNIT_ASSERT_EQUAL(std::string("ghi"), result[2]);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);
