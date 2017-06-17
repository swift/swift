/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXParticipantParserTest, XEP0369_Example1_ParticipantWithNick) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<participant xmlns='urn:xmpp:mix:0'> <nick>thirdwitch</nick> </participant>"
                ));

    MIXParticipant::ref payload = parser.getPayload<MIXParticipant>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getNick());
    std::string nick = *payload->getNick();
    ASSERT_EQ("thirdwitch", nick);

    ASSERT_FALSE(payload->getJID());
}

TEST(MIXParticipantParserTest, XEP0369_Example2_ParticipantWithJID) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<participant xmlns='urn:xmpp:mix:0'> <jid>hecate@mix.shakespeare.example</jid> </participant>"
                ));

    MIXParticipant::ref payload = parser.getPayload<MIXParticipant>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getJID());
    JID jid = *payload->getJID();
    ASSERT_EQ("hecate@mix.shakespeare.example", jid.toString());

    ASSERT_FALSE(payload->getNick());
}

TEST(MIXParticipantParserTest, XEP0369_Example27_ParticipantEmpty) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<participant xmlns='urn:xmpp:mix:0'/>"
                ));

    MIXParticipant::ref payload = parser.getPayload<MIXParticipant>();
    ASSERT_TRUE(payload);
    ASSERT_FALSE(payload->getNick());
    ASSERT_FALSE(payload->getJID());
}



