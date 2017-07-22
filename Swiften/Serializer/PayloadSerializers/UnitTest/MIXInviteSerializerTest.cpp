/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXInviteSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXInviteSerializerTest, testSerializeInvitee) {
    MIXInviteSerializer testling;

    auto invite = std::make_shared<MIXInvite>();
    invite->setInvitee(JID("cat@shakespeare.lit"));

    std::string expectedResult = "<invite xmlns=\"urn:xmpp:mix:0\">"
                                    "<invitee>cat@shakespeare.lit</invitee>"
                                "</invite>";

    ASSERT_EQ(expectedResult, testling.serialize(invite));
}

TEST(MIXInviteSerializerTest, testSerializeInvitation) {
    MIXInviteSerializer testling;

    auto invite = std::make_shared<MIXInvite>();

    auto invitation = std::make_shared<MIXInvitation>();
    invitation->setInviter(JID("hag66@shakespeare.lit"));
    invitation->setInvitee(JID("cat@shakespeare.lit"));
    invitation->setChannel(JID("coven@mix.shakespeare.lit"));
    invitation->setToken(std::string("ABCDEF"));

    invite->setInvitation(invitation);

    std::string expectedResult = "<invite xmlns=\"urn:xmpp:mix:0\">"
                                    "<invitation xmlns=\"urn:xmpp:mix:0\">"
                                        "<inviter>hag66@shakespeare.lit</inviter>"
                                        "<invitee>cat@shakespeare.lit</invitee>"
                                        "<channel>coven@mix.shakespeare.lit</channel>"
                                        "<token>ABCDEF</token>"
                                    "</invitation>"
                                "</invite>";

    ASSERT_EQ(expectedResult, testling.serialize(invite));
}
