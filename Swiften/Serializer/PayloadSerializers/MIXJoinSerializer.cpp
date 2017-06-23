/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MIXJoinSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/MIXSubscribeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

MIXJoinSerializer::MIXJoinSerializer() {
}

MIXJoinSerializer::~MIXJoinSerializer() {
}

std::string MIXJoinSerializer::serializePayload(std::shared_ptr<MIXJoin> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("join", "urn:xmpp:mix:0");
    if (payload->getChannel()) {
        element.setAttribute("channel", *payload->getChannel());
    }
    if (payload->getJID()) {
        element.setAttribute("jid", *payload->getJID());
    }
    for (const auto& item : payload->getSubscriptions()) {
        element.addNode(std::make_shared<XMLRawTextNode>(MIXSubscribeSerializer().serialize(item)));
    }
    if (payload->getForm()) {
        element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getForm())));
    }
    return element.serialize();
}
