/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXRetractSerializer.h>

#include <memory>
#include <string>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

MIXRetractSerializer::MIXRetractSerializer() {
}

MIXRetractSerializer::~MIXRetractSerializer() {
}

std::string MIXRetractSerializer::serializePayload(std::shared_ptr<MIXRetract> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("retract", "urn:xmpp:mix:0");
    element.setAttribute("id", payload->getMessageID());
    return element.serialize();
}

}
