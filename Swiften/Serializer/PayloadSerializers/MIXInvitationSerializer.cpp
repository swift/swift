/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXInvitationSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MIXInvitationSerializer::MIXInvitationSerializer() : GenericPayloadSerializer<MIXInvitation>() {
}

std::string MIXInvitationSerializer::serializePayload(std::shared_ptr<MIXInvitation> payload)  const {
    XMLElement invitationElement("invitation", "urn:xmpp:mix:0");

    auto inviterElement = std::make_shared<XMLElement>("inviter");
    inviterElement->addNode(std::make_shared<XMLTextNode>(payload->getInviter()));
    invitationElement.addNode(inviterElement);

    auto inviteeElement = std::make_shared<XMLElement>("invitee");
    inviteeElement->addNode(std::make_shared<XMLTextNode>(payload->getInvitee()));
    invitationElement.addNode(inviteeElement);

    auto channelElement = std::make_shared<XMLElement>("channel");
    channelElement->addNode(std::make_shared<XMLTextNode>(payload->getChannel()));
    invitationElement.addNode(channelElement);

    auto tokenElement = std::make_shared<XMLElement>("token");
    tokenElement->addNode(std::make_shared<XMLTextNode>(payload->getToken()));
    invitationElement.addNode(tokenElement);

    return invitationElement.serialize();
}

}
