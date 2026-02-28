/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXSetNickSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXSetNickSerializerTest, testSerializeNick) {
    MIXSetNickSerializer testling;

    auto mix = std::shared_ptr<MIXSetNick>(new MIXSetNick());
    mix->setNick(std::string("thirdwitch"));

    std::string expectedResult = "<setnick xmlns=\"urn:xmpp:mix:0\"><nick>thirdwitch</nick></setnick>";
    ASSERT_EQ(expectedResult, testling.serialize(mix));
}
