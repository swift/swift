/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubSubscribeSerializer::PubSubSubscribeSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubSubscribeSerializer::~PubSubSubscribeSerializer() {
}

std::string PubSubSubscribeSerializer::serializePayload(std::shared_ptr<PubSubSubscribe> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscribe", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    element.setAttribute("jid", payload->getJID());
    return element.serialize();
}


