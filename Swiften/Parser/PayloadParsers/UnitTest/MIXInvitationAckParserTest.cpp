/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXInvitationAck.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXInvitationAckParserTest, testParseInvitation) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<invitation-ack xmlns=\"urn:xmpp:mix:0\">"
                    "<value>Declined</value>"
                    "<invitation>"
                        "<inviter>hag66@shakespeare.lit</inviter>"
                        "<invitee>cat@shakespeare.lit</invitee>"
                        "<channel>coven@mix.shakespeare.lit</channel>"
                        "<token>ABCDEF</token>"
                    "</invitation>"
                "</invitation-ack>"
                ));

    auto payload = parser.getPayload<MIXInvitationAck>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getInvitation());
    auto invite = payload->getInvitation();
    ASSERT_EQ(invite->getInviter(), JID("hag66@shakespeare.lit"));
    ASSERT_EQ(invite->getInvitee(), JID("cat@shakespeare.lit"));
    ASSERT_EQ(invite->getChannel(), JID("coven@mix.shakespeare.lit"));
    ASSERT_EQ(invite->getToken(), std::string("ABCDEF"));

    ASSERT_EQ(payload->getInvitationStatus(), MIXInvitationAck::InvitationStatus::Declined);
}
