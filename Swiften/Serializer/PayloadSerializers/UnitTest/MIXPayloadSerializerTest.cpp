/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXPayloadSerializerTest, testSerializeEmpty) {
    MIXPayloadSerializer testling;

    auto mix = std::shared_ptr<MIXPayload>(new MIXPayload());

    std::string expectedResult = "<mix xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(mix));
}

TEST(MIXPayloadSerializerTest, testSerializeNick) {
    MIXPayloadSerializer testling;

    auto mix = std::shared_ptr<MIXPayload>(new MIXPayload());
    mix->setNick("thirdwitch");

    std::string expectedResult = "<mix xmlns=\"urn:xmpp:mix:0\">"
                                    "<nick>thirdwitch</nick>"
                                  "</mix>";
    ASSERT_EQ(expectedResult, testling.serialize(mix));
}

TEST(MIXPayloadSerializerTest, testSerializeJID) {
    MIXPayloadSerializer testling;

    auto mix = std::shared_ptr<MIXPayload>(new MIXPayload());
    mix->setJID(JID("hecate@mix.shakespeare.example"));

    std::string expectedResult = "<mix xmlns=\"urn:xmpp:mix:0\">"
                                    "<jid>hecate@mix.shakespeare.example</jid>"
                                  "</mix>";
    ASSERT_EQ(expectedResult, testling.serialize(mix));
}

TEST(MIXPayloadSerializerTest, testSerializeAll) {
    MIXPayloadSerializer testling;

    auto mix = std::shared_ptr<MIXPayload>(new MIXPayload());
    mix->setNick("thirdwitch");
    mix->setJID(JID("hecate@mix.shakespeare.example"));
    mix->setSubmissionID("92vax143g");

    std::string expectedResult = "<mix xmlns=\"urn:xmpp:mix:0\">"
                      "<nick>thirdwitch</nick>"
                      "<jid>hecate@mix.shakespeare.example</jid>"
                      "<submission-id>92vax143g</submission-id>"
                  "</mix>";
    ASSERT_EQ(expectedResult, testling.serialize(mix));
}
