/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCInvitationPayloadSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/MUCItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MUCInvitationPayloadSerializer::MUCInvitationPayloadSerializer() : GenericPayloadSerializer<MUCInvitationPayload>() {
}

std::string MUCInvitationPayloadSerializer::serializePayload(std::shared_ptr<MUCInvitationPayload> payload)  const {
    XMLElement mucElement("x", "jabber:x:conference");
    if (payload->getIsContinuation()) {
        mucElement.setAttribute("continue", "true");
    }
    if (payload->getJID().isValid()) {
        mucElement.setAttribute("jid", payload->getJID().toString());
    }
    if (!payload->getPassword().empty()) {
        mucElement.setAttribute("password", payload->getPassword());
    }
    if (!payload->getReason().empty()) {
        mucElement.setAttribute("reason", payload->getReason());
    }
    if (!payload->getThread().empty()) {
        mucElement.setAttribute("thread", payload->getThread());
    }
    if (payload->getIsImpromptu()) {
        mucElement.addNode(std::make_shared<XMLElement>("impromptu", "http://swift.im/impromptu"));
    }
    return mucElement.serialize();
}


}
