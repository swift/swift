/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationAckSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXInvitationAckSerializerTest, testSerializeInvitation) {
    MIXInvitationAckSerializer testling;

    auto invitation = std::make_shared<MIXInvitationAck>();

    auto invite = std::make_shared<MIXInvitation>();
    invite->setInviter(JID("hag66@shakespeare.lit"));
    invite->setInvitee(JID("cat@shakespeare.lit"));
    invite->setChannel(JID("coven@mix.shakespeare.lit"));
    invite->setToken(std::string("ABCDEF"));

    invitation->setInvitation(invite);
    invitation->setInvitationStatus(MIXInvitationAck::InvitationStatus::Joined);

    std::string expectedResult = "<invitation-ack xmlns=\"urn:xmpp:mix:0\">"
                    "<value>Joined</value>"
                    "<invitation xmlns=\"urn:xmpp:mix:0\">"
                        "<inviter>hag66@shakespeare.lit</inviter>"
                        "<invitee>cat@shakespeare.lit</invitee>"
                        "<channel>coven@mix.shakespeare.lit</channel>"
                        "<token>ABCDEF</token>"
                    "</invitation>"
                "</invitation-ack>";

    ASSERT_EQ(expectedResult, testling.serialize(invitation));
}
