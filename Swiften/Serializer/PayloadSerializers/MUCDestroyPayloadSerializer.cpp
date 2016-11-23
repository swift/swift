/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCDestroyPayloadSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MUCDestroyPayloadSerializer::MUCDestroyPayloadSerializer() : GenericPayloadSerializer<MUCDestroyPayload>() {
}

std::string MUCDestroyPayloadSerializer::serializePayload(std::shared_ptr<MUCDestroyPayload> payload)  const {
    XMLElement mucElement("destroy", "");
    if (!payload->getReason().empty()) {
        XMLElement::ref reason = std::make_shared<XMLElement>("reason", "");
        reason->addNode(std::make_shared<XMLTextNode>(payload->getReason()));
        mucElement.addNode(reason);
    }
    if (payload->getNewVenue().isValid()) {
        mucElement.setAttribute("jid", payload->getNewVenue().toString());
    }
    return mucElement.serialize();
}




}
