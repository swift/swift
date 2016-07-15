/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubCreateSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubCreateSerializer::PubSubCreateSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubCreateSerializer::~PubSubCreateSerializer() {
}

std::string PubSubCreateSerializer::serializePayload(std::shared_ptr<PubSubCreate> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("create", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


