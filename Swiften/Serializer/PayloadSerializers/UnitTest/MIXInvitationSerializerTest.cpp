/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <gtest/gtest.h>

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>

using namespace Swift;

TEST(MIXInvitationSerializerTest, testSerializeInvitation) {
    MIXInvitationSerializer testling;

    auto invitation = std::make_shared<MIXInvitation>();
    invitation->setInviter(JID("hag66@shakespeare.lit"));
    invitation->setInvitee(JID("cat@shakespeare.lit"));
    invitation->setChannel(JID("coven@mix.shakespeare.lit"));
    invitation->setToken(std::string("ABCDEF"));

    std::string expectedResult = "<invitation xmlns=\"urn:xmpp:mix:0\">"
                    "<inviter>hag66@shakespeare.lit</inviter>"
                    "<invitee>cat@shakespeare.lit</invitee>"
                    "<channel>coven@mix.shakespeare.lit</channel>"
                    "<token>ABCDEF</token>"
                "</invitation>";

    ASSERT_EQ(expectedResult, testling.serialize(invitation));
}
