/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXCreateSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MIXCreateSerializer::MIXCreateSerializer() {
}

MIXCreateSerializer::~MIXCreateSerializer() {
}

std::string MIXCreateSerializer::serializePayload(std::shared_ptr<MIXCreate> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("create", "urn:xmpp:mix:0");

    if(payload->getChannel()) {
        element.setAttribute("channel", *payload->getChannel());
    }

    if(payload->getData()) {
        element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    }

    return element.serialize();
}
