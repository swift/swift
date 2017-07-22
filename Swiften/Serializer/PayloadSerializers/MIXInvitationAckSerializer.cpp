/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationAckSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MIXInvitationAckSerializer::MIXInvitationAckSerializer() : GenericPayloadSerializer<MIXInvitationAck>() {
}

std::string MIXInvitationAckSerializer::serializePayload(std::shared_ptr<MIXInvitationAck> payload)  const {
    XMLElement invitationElement("invitation-ack", "urn:xmpp:mix:0");

    auto valueElement = std::make_shared<XMLElement>("value");
    MIXInvitationAck::InvitationStatus status = payload->getInvitationStatus();

    std::string valueString;
    if (status == MIXInvitationAck::InvitationStatus::Declined) {
        valueString = "Declined";
    } else if (status == MIXInvitationAck::InvitationStatus::Joined) {
        valueString = "Joined";
    } else if (status == MIXInvitationAck::InvitationStatus::Acknowledged) {
        valueString = "Acknowledged";
    }

    valueElement->addNode(std::make_shared<XMLTextNode>(valueString));
    invitationElement.addNode(valueElement);

    invitationElement.addNode(std::make_shared<XMLRawTextNode>(MIXInvitationSerializer().serialize(payload->getInvitation())));

    return invitationElement.serialize();
}

}
