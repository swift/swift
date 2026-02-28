/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXCreateSerializer.h>

using namespace Swift;

TEST(MIXCreateSerializerTest, XEP0369_Example31) {
    MIXCreateSerializer testling;

    auto create = std::make_shared<MIXCreate>();
    std::string expectedResult = "<create xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(create));
}

TEST(MIXCreateSerializerTest, XEP0369_Example66) {
    MIXCreateSerializer testling;

    auto create = std::make_shared<MIXCreate>();
    create->setChannel(std::string("coven"));

    std::string expectedResult = "<create channel=\"coven\" xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(create));
}
