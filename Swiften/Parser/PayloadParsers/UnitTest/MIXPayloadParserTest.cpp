/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXPayloadParserTest, WithNick) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<mix xmlns=\"urn:xmpp:mix:0\"> <nick>thirdwitch</nick> </mix>"
                ));

    auto payload = parser.getPayload<MIXPayload>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getNick());
    std::string nick = *payload->getNick();
    ASSERT_EQ("thirdwitch", nick);

    ASSERT_FALSE(payload->getJID());
    ASSERT_FALSE(payload->getSubmissionID());
}

TEST(MIXPayloadParserTest, WithJID) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<mix xmlns=\"urn:xmpp:mix:0\"> <jid>hecate@mix.shakespeare.example</jid> </mix>"
                ));

    auto payload = parser.getPayload<MIXPayload>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getJID());
    JID jid = *payload->getJID();
    ASSERT_EQ("hecate@mix.shakespeare.example", jid.toString());

    ASSERT_FALSE(payload->getNick());
    ASSERT_FALSE(payload->getSubmissionID());
}

TEST(MIXPayloadParserTest, WithAll) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<mix xmlns=\"urn:xmpp:mix:0\">"
                    "<nick>thirdwitch</nick>"
                    "<jid>hecate@mix.shakespeare.example</jid>"
                    "<submission-id>92vax143g</submission-id>"
                "</mix>"
                ));

    auto payload = parser.getPayload<MIXPayload>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getNick());
    std::string nick = *payload->getNick();
    ASSERT_EQ("thirdwitch", nick);

    ASSERT_TRUE(payload->getJID());
    JID jid = *payload->getJID();
    ASSERT_EQ("hecate@mix.shakespeare.example", jid.toString());

    ASSERT_TRUE(payload->getSubmissionID());
    std::string subID = *payload->getSubmissionID();
    ASSERT_EQ("92vax143g", subID);
}

TEST(MIXPayloadParserTest, Empty) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<mix xmlns=\"urn:xmpp:mix:0\"/>"
                ));

    auto payload = parser.getPayload<MIXPayload>();
    ASSERT_TRUE(payload);
    ASSERT_FALSE(payload->getNick());
    ASSERT_FALSE(payload->getJID());
    ASSERT_FALSE(payload->getSubmissionID());
}
