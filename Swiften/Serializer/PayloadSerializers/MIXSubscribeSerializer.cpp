/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXSubscribeSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MIXSubscribeSerializer::MIXSubscribeSerializer() {
}

MIXSubscribeSerializer::~MIXSubscribeSerializer() {
}

std::string MIXSubscribeSerializer::serializePayload(std::shared_ptr<MIXSubscribe> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscribe");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}
