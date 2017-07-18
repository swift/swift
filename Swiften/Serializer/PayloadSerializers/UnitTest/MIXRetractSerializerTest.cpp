/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXRetractSerializer.h>

using namespace Swift;

TEST(MIXRetractSerializerTest, testID) {
    MIXRetractSerializer testling;

    auto retract = std::make_shared<MIXRetract>();
    retract->setMessageID(std::string("28482-98726-73623"));

    std::string expectedResult = "<retract id=\"28482-98726-73623\" xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(retract));
}
