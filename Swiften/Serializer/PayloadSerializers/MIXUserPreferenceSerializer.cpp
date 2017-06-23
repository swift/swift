/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXUserPreferenceSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

MIXUserPreferenceSerializer::MIXUserPreferenceSerializer() {
}

MIXUserPreferenceSerializer::~MIXUserPreferenceSerializer() {
}

std::string MIXUserPreferenceSerializer::serializePayload(std::shared_ptr<MIXUserPreference> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("user-preference", "urn:xmpp:mix:0");
    if(payload->getData()) {
        element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    }
    return element.serialize();
}
}
