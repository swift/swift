/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXDestroySerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MIXDestroySerializer::MIXDestroySerializer() {
}

MIXDestroySerializer::~MIXDestroySerializer() {
}

std::string MIXDestroySerializer::serializePayload(std::shared_ptr<MIXDestroy> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("destroy", "urn:xmpp:mix:0");
    element.setAttribute("channel", payload->getChannel());
    return element.serialize();
}
