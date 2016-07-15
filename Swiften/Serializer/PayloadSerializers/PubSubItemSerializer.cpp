/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>

#include <memory>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubItemSerializer::PubSubItemSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubItemSerializer::~PubSubItemSerializer() {
}

std::string PubSubItemSerializer::serializePayload(std::shared_ptr<PubSubItem> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("item", "http://jabber.org/protocol/pubsub");
    foreach(std::shared_ptr<Payload> item, payload->getData()) {
        element.addNode(std::make_shared<XMLRawTextNode>(serializers->getPayloadSerializer(item)->serialize(item)));
    }
    if (!payload->getID().empty()) {
        element.setAttribute("id", payload->getID());
    }
    return element.serialize();
}


