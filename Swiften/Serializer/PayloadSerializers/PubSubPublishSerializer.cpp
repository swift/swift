/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubPublishSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubPublishSerializer::PubSubPublishSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubPublishSerializer::~PubSubPublishSerializer() {
}

std::string PubSubPublishSerializer::serializePayload(std::shared_ptr<PubSubPublish> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("publish", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    foreach(std::shared_ptr<PubSubItem> item, payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


