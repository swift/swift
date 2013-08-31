/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>
#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

class Base64Test : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(Base64Test);
		CPPUNIT_TEST(testEncodeDecodeAllChars);
		CPPUNIT_TEST(testEncodeDecodeOneBytePadding);
		CPPUNIT_TEST(testEncodeDecodeTwoBytesPadding);
		CPPUNIT_TEST(testEncode_NoData);
		CPPUNIT_TEST(testDecode_NoData);
		CPPUNIT_TEST(testDecode_DataWithNewlines);
		CPPUNIT_TEST_SUITE_END();

	public:
		void testEncodeDecodeAllChars() {
			ByteArray input;
			for (unsigned char i = 0; i < 255; ++i) {
				input.push_back(i);
			}
			std::string result(Base64::encode(input));

			CPPUNIT_ASSERT_EQUAL(std::string("AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+"), result);
			CPPUNIT_ASSERT_EQUAL(input, Base64::decode(result));
		}

		void testEncodeDecodeOneBytePadding() {
			ByteArray input = createByteArray("ABCDE", 5);

			std::string result = Base64::encode(input);

			CPPUNIT_ASSERT_EQUAL(std::string("QUJDREU="), result);
			CPPUNIT_ASSERT_EQUAL(input, Base64::decode(result));
		}

		void testEncodeDecodeTwoBytesPadding() {
			ByteArray input = createByteArray("ABCD", 4);

			std::string result = Base64::encode(input);

			CPPUNIT_ASSERT_EQUAL(std::string("QUJDRA=="), result);
			CPPUNIT_ASSERT_EQUAL(input, Base64::decode(result));
		}

		void testEncode_NoData() {
			std::string result(Base64::encode(ByteArray()));
			CPPUNIT_ASSERT_EQUAL(std::string(""), result);
		}

		void testDecode_NoData() {
			ByteArray result(Base64::decode(""));
			CPPUNIT_ASSERT_EQUAL(ByteArray(), result);
		}

		void testDecode_DataWithNewlines() {
			ByteArray result(Base64::decode("dTsETKSAskgu2/BqVO+ogcu3DJy4QATGJqpsa6znWwNGiLnVElVVB6PtS+mTiHUXsrOlKvRjtvzV\n"
											"VDknNaRF58Elmu5EC6VoCllBEEB/lFf0emYn2gkp0X1khNi75dl+rOj95Ar6XuwLh+ZoSStqwOWj\n"
											"pIpxmZmVw7E69qr0FY0oI3zcaxXwzHw7Lx9Qf4sH7ufQvIN88ga+hwp8MiXevh3Ac8pN00kgINlq\n"
											"9AY/bYJL418Y/6wWsJbgmrJ/N78wSMpC7VVszLBZVv8uFnupubyi8Ophd/1wIWWzPPwAbBhepWVb\n"
											"1oPiFEBT5MNKCMTPEi0npXtedVz0HQbbPNIVwmo="));
			CPPUNIT_ASSERT_EQUAL(createByteArray("\x75\x3B\x04\x4C\xA4\x80\xB2\x48\x2E\xDB\xF0\x6A\x54\xEF\xA8\x81\xCB\xB7\x0C\x9C\xB8\x40\x04\xC6\x26\xAA\x6C\x6B\xAC\xE7\x5B\x03\x46\x88\xB9\xD5\x12\x55\x55\x07\xA3\xED\x4B\xE9\x93\x88\x75\x17\xB2\xB3\xA5\x2A\xF4\x63\xB6\xFC\xD5\x54\x39\x27\x35\xA4\x45\xE7\xC1\x25\x9A\xEE\x44\x0B\xA5\x68\x0A\x59\x41\x10\x40\x7F\x94\x57\xF4\x7A\x66\x27\xDA\x09\x29\xD1\x7D\x64\x84\xD8\xBB\xE5\xD9\x7E\xAC\xE8\xFD\xE4\x0A\xFA\x5E\xEC\x0B\x87\xE6\x68\x49\x2B\x6A\xC0\xE5\xA3\xA4\x8A\x71\x99\x99\x95\xC3\xB1\x3A\xF6\xAA\xF4\x15\x8D\x28\x23\x7C\xDC\x6B\x15\xF0\xCC\x7C\x3B\x2F\x1F\x50\x7F\x8B\x07\xEE\xE7\xD0\xBC\x83\x7C\xF2\x06\xBE\x87\x0A\x7C\x32\x25\xDE\xBE\x1D\xC0\x73\xCA\x4D\xD3\x49\x20\x20\xD9\x6A\xF4\x06\x3F\x6D\x82\x4B\xE3\x5F\x18\xFF\xAC\x16\xB0\x96\xE0\x9A\xB2\x7F\x37\xBF\x30\x48\xCA\x42\xED\x55\x6C\xCC\xB0\x59\x56\xFF\x2E\x16\x7B\xA9\xB9\xBC\xA2\xF0\xEA\x61\x77\xFD\x70\x21\x65\xB3\x3C\xFC\x00\x6C\x18\x5E\xA5\x65\x5B\xD6\x83\xE2\x14\x40\x53\xE4\xC3\x4A\x08\xC4\xCF\x12\x2D\x27\xA5\x7B\x5E\x75\x5C\xF4\x1D\x06\xDB\x3C\xD2\x15\xC2\x6A", 257), result);
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Base64Test);
