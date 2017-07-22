/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXInvite.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXInviteParserTest, testParseInvitee) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<invite xmlns=\"urn:xmpp:mix:0\">"
                    "<invitee>cat@shakespeare.lit</invitee>"
                "</invite>"
                ));

    auto payload = parser.getPayload<MIXInvite>();
    ASSERT_TRUE(payload);

    ASSERT_FALSE(payload->getInvitation());
    ASSERT_TRUE(payload->getInvitee());
    ASSERT_EQ(*payload->getInvitee(), JID("cat@shakespeare.lit"));
}

TEST(MIXInviteParserTest, testParseInvitation) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<invite xmlns=\"urn:xmpp:mix:0\">"
                    "<invitation>"
                        "<inviter>hag66@shakespeare.lit</inviter>"
                        "<invitee>cat@shakespeare.lit</invitee>"
                        "<channel>coven@mix.shakespeare.lit</channel>"
                        "<token>ABCDEF</token>"
                    "</invitation>"
                "</invite>"
                ));

    auto payload = parser.getPayload<MIXInvite>();
    ASSERT_TRUE(payload);

    ASSERT_TRUE(payload->getInvitation());
    ASSERT_FALSE(payload->getInvitee());

    auto invite = *payload->getInvitation();
    ASSERT_EQ(invite->getInviter(), JID("hag66@shakespeare.lit"));
    ASSERT_EQ(invite->getInvitee(), JID("cat@shakespeare.lit"));
    ASSERT_EQ(invite->getChannel(), JID("coven@mix.shakespeare.lit"));
    ASSERT_EQ(invite->getToken(), std::string("ABCDEF"));
}
