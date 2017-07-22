/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Elements/MIXInvitation.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>

using namespace Swift;

TEST(MIXInvitationParserTest, testParseInvitation) {
    PayloadsParserTester parser;
    ASSERT_TRUE(parser.parse(
                "<invitation xmlns=\"urn:xmpp:mix:0\">"
                    "<inviter>hag66@shakespeare.lit</inviter>"
                    "<invitee>cat@shakespeare.lit</invitee>"
                    "<channel>coven@mix.shakespeare.lit</channel>"
                    "<token>ABCDEF</token>"
                "</invitation>"
                ));

    auto payload = parser.getPayload<MIXInvitation>();
    ASSERT_TRUE(payload);

    ASSERT_EQ(payload->getInviter(), JID("hag66@shakespeare.lit"));
    ASSERT_EQ(payload->getInvitee(), JID("cat@shakespeare.lit"));
    ASSERT_EQ(payload->getChannel(), JID("coven@mix.shakespeare.lit"));
    ASSERT_EQ(payload->getToken(), std::string("ABCDEF"));
}
