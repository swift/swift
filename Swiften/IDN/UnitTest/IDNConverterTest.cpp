/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <memory>

#include <gtest/gtest.h>

#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>

using namespace Swift;

class IDNConverterTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        testling_ = std::shared_ptr<IDNConverter>(PlatformIDNConverter::create());
    }

    std::shared_ptr<IDNConverter> testling_;
};

TEST_F(IDNConverterTest, testStringPrep) {
    std::string result = testling_->getStringPrepared("tron\xc3\x87on", IDNConverter::NamePrep);

    ASSERT_EQ(std::string("tron\xc3\xa7on"), result);
}

TEST_F(IDNConverterTest, testStringPrep_Empty) {
    ASSERT_EQ(std::string(""), testling_->getStringPrepared("", IDNConverter::NamePrep));
    ASSERT_EQ(std::string(""), testling_->getStringPrepared("", IDNConverter::XMPPNodePrep));
    ASSERT_EQ(std::string(""), testling_->getStringPrepared("", IDNConverter::XMPPResourcePrep));
}

TEST_F(IDNConverterTest, testStringPrep_MaximumOutputSize) {
    const std::string input(1023, 'x');
    ASSERT_EQ(input, testling_->getStringPrepared(input, IDNConverter::NamePrep));
    ASSERT_EQ(input, testling_->getStringPrepared(input, IDNConverter::XMPPNodePrep));
    ASSERT_EQ(input, testling_->getStringPrepared(input, IDNConverter::XMPPResourcePrep));
}

TEST_F(IDNConverterTest, testStringPrep_TooLong) {
    const std::string input(1024, 'x');
    ASSERT_THROW(testling_->getStringPrepared(input, IDNConverter::NamePrep), std::exception);
    ASSERT_THROW(testling_->getStringPrepared(input, IDNConverter::XMPPNodePrep), std::exception);
    ASSERT_THROW(testling_->getStringPrepared(input, IDNConverter::XMPPResourcePrep), std::exception);
}

TEST_F(IDNConverterTest, testStringPrep_ShrinkingBelow1023) {
    std::string input;
    std::string expected;
    // The four byte \u03b1\u0313 UTF-8 string will shrink to the three byte \u1f00
    for (auto i = 0; i < 300; ++i) {
        input +="\xce\xb1\xcc\x93"; // UTF-8 repesentation of U+03B1 U+0313
        expected += "\xe1\xbc\x80"; // UTF-8 representation of U+1F00
    }
    ASSERT_EQ(expected, testling_->getStringPrepared(input, IDNConverter::NamePrep));
    ASSERT_EQ(expected, testling_->getStringPrepared(input, IDNConverter::XMPPNodePrep));
    ASSERT_EQ(expected, testling_->getStringPrepared(input, IDNConverter::XMPPResourcePrep));
}

TEST_F(IDNConverterTest, testGetEncoded) {
    boost::optional<std::string> result = testling_->getIDNAEncoded("www.swift.im");
    ASSERT_TRUE(!!result);
    ASSERT_EQ(std::string("www.swift.im"), *result);
}

TEST_F(IDNConverterTest, testGetEncoded_International) {
    boost::optional<std::string> result = testling_->getIDNAEncoded("www.tron\xc3\x87on.com");
    ASSERT_TRUE(result);
    ASSERT_EQ(std::string("www.xn--tronon-zua.com"), *result);
}

TEST_F(IDNConverterTest, testGetEncoded_Invalid) {
    boost::optional<std::string> result = testling_->getIDNAEncoded("www.foo,bar.com");
    ASSERT_FALSE(result);
}

TEST_F(IDNConverterTest, testRFC1035LengthRestrictions) {
    // label size check, 63 octets or less
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + ".example"));
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(63, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(64, 'a') + "." + std::string(63, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(64, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(0, 'a') + "." + std::string(63, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(0, 'a') + ".example"));

    // domain name 255 octets or less
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + ".example"));
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(63, 'a') + ".example"));
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(63, 'a') + "." + std::string(63, 'a') + ".example"));
    ASSERT_TRUE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(63, 'a') + "." + std::string(63, 'a') + "." + std::string(55, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(63, 'a') + "." + std::string(63, 'a') + "." + std::string(56, 'a') + ".example"));
    ASSERT_FALSE(testling_->getIDNAEncoded(std::string(63, 'a') + "." + std::string(56, 'a') + "." + std::string(63, 'a') + "." + std::string(63, 'a') + ".example"));
}
