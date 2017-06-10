/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXParticipantSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXParticipantSerializerTest, testSerializeEmpty) {
    MIXParticipantSerializer testling;

    std::shared_ptr<MIXParticipant> participant(new MIXParticipant());

    std::string expectedResult = "<participant xmlns=\"urn:xmpp:mix:0\"/>";
    ASSERT_EQ(expectedResult, testling.serialize(participant));
}

TEST(MIXParticipantSerializerTest, testSerializeNick) {
    MIXParticipantSerializer testling;

    std::shared_ptr<MIXParticipant> participant(new MIXParticipant());
    participant->setNick("thirdwitch");

    std::string expectedResult = "<participant xmlns=\"urn:xmpp:mix:0\">"
                      "<nick>thirdwitch</nick>"
                  "</participant>";
    ASSERT_EQ(expectedResult, testling.serialize(participant));
}

TEST(MIXParticipantSerializerTest, testSerializeJID) {
    MIXParticipantSerializer testling;

    std::shared_ptr<MIXParticipant> participant(new MIXParticipant());
    participant->setJID(JID("hecate@mix.shakespeare.example"));

    std::string expectedResult = "<participant xmlns=\"urn:xmpp:mix:0\">"
                      "<jid>hecate@mix.shakespeare.example</jid>"
                  "</participant>";
    ASSERT_EQ(expectedResult, testling.serialize(participant));
}

TEST(MIXParticipantSerializerTest, testSerializeJIDAndNick) {
    MIXParticipantSerializer testling;

    std::shared_ptr<MIXParticipant> participant(new MIXParticipant());
    participant->setNick("thirdwitch");
    participant->setJID(JID("hecate@mix.shakespeare.example"));

    std::string expectedResult = "<participant xmlns=\"urn:xmpp:mix:0\">"
                      "<nick>thirdwitch</nick>"
                      "<jid>hecate@mix.shakespeare.example</jid>"
                  "</participant>";
    ASSERT_EQ(expectedResult, testling.serialize(participant));
}
