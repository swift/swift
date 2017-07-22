/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXInviteSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MIXInviteSerializer::MIXInviteSerializer() : GenericPayloadSerializer<MIXInvite>() {
}

std::string MIXInviteSerializer::serializePayload(std::shared_ptr<MIXInvite> payload)  const {
    XMLElement invitationElement("invite", "urn:xmpp:mix:0");

    if (payload->getInvitee()) {
        auto inviteeElement = std::make_shared<XMLElement>("invitee");
        inviteeElement->addNode(std::make_shared<XMLTextNode>(*payload->getInvitee()));
        invitationElement.addNode(inviteeElement);
    }

    if (payload->getInvitation()) {
        invitationElement.addNode(std::make_shared<XMLRawTextNode>(MIXInvitationSerializer().serialize(*payload->getInvitation())));
    }

    return invitationElement.serialize();
}

}
