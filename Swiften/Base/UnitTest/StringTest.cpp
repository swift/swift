#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Base/String.h"

using namespace Swift;

class StringTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StringTest);
		CPPUNIT_TEST(testGetLength);
		CPPUNIT_TEST(testGetLength_EncodedLength2);
		CPPUNIT_TEST(testGetLength_EncodedLength3);
		CPPUNIT_TEST(testGetLength_EncodedLength4);
		CPPUNIT_TEST(testGetUnicodeCodePoints);
		CPPUNIT_TEST(testGetSplittedAtFirst);
		CPPUNIT_TEST(testGetSplittedAtFirst_CharacterAtEnd);
		CPPUNIT_TEST(testGetSplittedAtFirst_NoSuchCharacter);
		CPPUNIT_TEST(testRemoveAll);
		CPPUNIT_TEST(testRemoveAll_LastChar);
		CPPUNIT_TEST(testRemoveAll_ConsecutiveChars);
		CPPUNIT_TEST(testReplaceAll);
		CPPUNIT_TEST(testReplaceAll_LastChar);
		CPPUNIT_TEST(testReplaceAll_ConsecutiveChars);
		CPPUNIT_TEST(testReplaceAll_MatchingReplace);
		CPPUNIT_TEST_SUITE_END();

	public:
		StringTest() {}

		void testGetLength() {
			String testling("xyz$xyz");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), testling.getLength());
		}

		void testGetLength_EncodedLength2() {
			String testling("xyz\xC2\xA2xyz");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), testling.getLength());
		}

		void testGetLength_EncodedLength3() {
			String testling("xyz\xE2\x82\xACxyz");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), testling.getLength());
		}

		void testGetLength_EncodedLength4() {
			String testling("xyz\xf4\x8a\xaf\x8dxyz");

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), testling.getLength());
		}

		void testGetUnicodeCodePoints() {
			String testling("$\xc2\xa2\xe2\x82\xac\xf4\x8a\xaf\x8d");
			std::vector<unsigned int> points = testling.getUnicodeCodePoints();

			CPPUNIT_ASSERT_EQUAL(0x24U, points[0]);
			CPPUNIT_ASSERT_EQUAL(0xA2U, points[1]);
			CPPUNIT_ASSERT_EQUAL(0x20ACU, points[2]);
			CPPUNIT_ASSERT_EQUAL(0x10ABCDU, points[3]);
		}

		void testGetSplittedAtFirst() {
			String testling("ab@cd@ef");

			std::pair<String,String> result = testling.getSplittedAtFirst('@');
			CPPUNIT_ASSERT_EQUAL(String("ab"), result.first);
			CPPUNIT_ASSERT_EQUAL(String("cd@ef"), result.second);
		}

		void testGetSplittedAtFirst_CharacterAtEnd() {
			String testling("ab@");

			std::pair<String,String> result = testling.getSplittedAtFirst('@');
			CPPUNIT_ASSERT_EQUAL(String("ab"), result.first);
			CPPUNIT_ASSERT(result.second.isEmpty());
		}

		void testGetSplittedAtFirst_NoSuchCharacter() {
			String testling("ab");

			std::pair<String,String> result = testling.getSplittedAtFirst('@');
			CPPUNIT_ASSERT_EQUAL(String("ab"), result.first);
			CPPUNIT_ASSERT(result.second.isEmpty());
		}

		void testRemoveAll() {
			String testling("ab c de");

			testling.removeAll(' ');
			
			CPPUNIT_ASSERT_EQUAL(String("abcde"), testling);
		}

		void testRemoveAll_LastChar() {
			String testling("abcde ");

			testling.removeAll(' ');
			
			CPPUNIT_ASSERT_EQUAL(String("abcde"), testling);
		}

		void testRemoveAll_ConsecutiveChars() {
			String testling("ab  cde");

			testling.removeAll(' ');
			
			CPPUNIT_ASSERT_EQUAL(String("abcde"), testling);
		}

		void testReplaceAll() {
			String testling("abcbd");

			testling.replaceAll('b', "xyz");
			
			CPPUNIT_ASSERT_EQUAL(String("axyzcxyzd"), testling);
		}

		void testReplaceAll_LastChar() {
			String testling("abc");

			testling.replaceAll('c', "xyz");
			
			CPPUNIT_ASSERT_EQUAL(String("abxyz"), testling);
		}

		void testReplaceAll_ConsecutiveChars() {
			String testling("abbc");

			testling.replaceAll('b',"xyz");
			
			CPPUNIT_ASSERT_EQUAL(String("axyzxyzc"), testling);
		}

		void testReplaceAll_MatchingReplace() {
			String testling("abc");

			testling.replaceAll('b',"bbb");
			
			CPPUNIT_ASSERT_EQUAL(String("abbbc"), testling);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);
