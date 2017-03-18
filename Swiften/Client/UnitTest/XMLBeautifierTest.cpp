/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include "gtest/gtest.h"
#include <Swiften/Client/XMLBeautifier.h>
#include <iostream>

using namespace Swift;

namespace {
    const static std::string FULL_FORMATTED_OUTPUT("<list>\n <el>aqq</el>\n <el>bzz</el>\n</list>");
}

TEST(XMLBeautifierTest, testBeautify) {
    auto beautifier = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, false));

    ASSERT_EQ(FULL_FORMATTED_OUTPUT, beautifier->beautify("<list><el>aqq</el><el>bzz</el></list>"));
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());
    ASSERT_EQ(FULL_FORMATTED_OUTPUT, beautifier->beautify("<list><el>aqq</el><el>bzz</el></list>"));
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());
}

TEST(XMLBeautifierTest, testBeautifyMultipleChunks) {
    auto beautifier = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, false));

    auto result = beautifier->beautify("<list><el>aqq</el>");
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(1, beautifier->getLevel());

    result += beautifier->beautify("<el>bzz</el></list>");
    ASSERT_FALSE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());

    ASSERT_EQ(FULL_FORMATTED_OUTPUT, result);
}

TEST(XMLBeautifierTest, testBeautifyMultipleChunksMiddleElement) {
    auto beautifier = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, false));

    auto result = beautifier->beautify("<l");
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());

    result += beautifier->beautify("ist><el>aqq</el><el>bzz</el></list>");
    ASSERT_FALSE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());

    ASSERT_EQ(FULL_FORMATTED_OUTPUT, result);
}

TEST(XMLBeautifierTest, testBeautifyInvalidMultipleChunks) {
    auto beautifier = std::unique_ptr<XMLBeautifier>(new XMLBeautifier(true, false));

    ASSERT_EQ(std::string("<list>\n <el>aqq"), beautifier->beautify("<list><el>aqq<"));
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(2, beautifier->getLevel());

    ASSERT_EQ(FULL_FORMATTED_OUTPUT, beautifier->beautify("<list><el>aqq</el><el>bzz</el></list>"));
    ASSERT_TRUE(beautifier->wasReset());
    ASSERT_EQ(0, beautifier->getLevel());
}
