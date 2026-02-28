/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <QA/Checker/IO.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

class Base64Test : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(Base64Test);
        CPPUNIT_TEST(testEncodeDecodeAllChars);
        CPPUNIT_TEST(testEncodeDecodeOneBytePadding);
        CPPUNIT_TEST(testEncodeDecodeTwoBytesPadding);
        CPPUNIT_TEST(testEncode_NoData);
        CPPUNIT_TEST(testDecode_NoData);
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(Base64Test);
