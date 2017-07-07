/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXLeaveSerializer.h>

using namespace Swift;

TEST(MIXLeaveSerializerTest, XEP0369_Example33) {
    MIXLeaveSerializer testling;

    auto leave = std::shared_ptr<MIXLeave>(new MIXLeave());
    leave->setChannel(JID("coven@mix.shakespeare.example"));

    std::string expectedResult = "<leave channel=\"coven@mix.shakespeare.example\" xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(leave));
}

TEST(MIXLeaveSerializerTest, XEP0369_Example34) {
    MIXLeaveSerializer testling;

    auto leave = std::shared_ptr<MIXLeave>(new MIXLeave());

    std::string expectedResult = "<leave xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(leave));
}
