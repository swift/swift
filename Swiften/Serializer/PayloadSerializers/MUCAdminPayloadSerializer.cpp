/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCAdminPayloadSerializer.h>

#include <memory>
#include <sstream>

#include <Swiften/Serializer/PayloadSerializers/MUCItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

MUCAdminPayloadSerializer::MUCAdminPayloadSerializer() : GenericPayloadSerializer<MUCAdminPayload>() {
}

std::string MUCAdminPayloadSerializer::serializePayload(std::shared_ptr<MUCAdminPayload> payload)  const {
    XMLElement mucElement("query", "http://jabber.org/protocol/muc#admin");
    for (const auto& item : payload->getItems()) {
        mucElement.addNode(MUCItemSerializer::itemToElement(item));
    }
    return mucElement.serialize();
}


}
