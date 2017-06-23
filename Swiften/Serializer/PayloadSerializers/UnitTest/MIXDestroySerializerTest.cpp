/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXDestroySerializer.h>

using namespace Swift;

TEST(MIXDestroySerializerTest, XEP0369_Example31) {
    MIXDestroySerializer testling;

    auto destroy = std::make_shared<MIXDestroy>();
    destroy->setChannel(std::string("coven"));

    std::string expectedResult = "<destroy channel=\"coven\" xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(destroy));
}
