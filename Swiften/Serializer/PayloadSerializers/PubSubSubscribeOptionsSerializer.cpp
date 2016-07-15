/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeOptionsSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubSubscribeOptionsSerializer::PubSubSubscribeOptionsSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubSubscribeOptionsSerializer::~PubSubSubscribeOptionsSerializer() {
}

std::string PubSubSubscribeOptionsSerializer::serializePayload(std::shared_ptr<PubSubSubscribeOptions> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscribe-options", "http://jabber.org/protocol/pubsub");
    element.addNode(payload->isRequired() ? std::make_shared<XMLElement>("required", "") : std::shared_ptr<XMLElement>());
    return element.serialize();
}


