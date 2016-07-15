/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDefaultSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerDefaultSerializer::PubSubOwnerDefaultSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubOwnerDefaultSerializer::~PubSubOwnerDefaultSerializer() {
}

std::string PubSubOwnerDefaultSerializer::serializePayload(std::shared_ptr<PubSubOwnerDefault> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("default", "http://jabber.org/protocol/pubsub#owner");
    element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    return element.serialize();
}
