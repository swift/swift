/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemsSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventRetractSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventItemsSerializer::PubSubEventItemsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventItemsSerializer::~PubSubEventItemsSerializer() {
}

std::string PubSubEventItemsSerializer::serializePayload(std::shared_ptr<PubSubEventItems> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("items", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    for (const auto& item : payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubEventItemSerializer(serializers).serialize(item)));
    }
    for (const auto& item : payload->getRetracts()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubEventRetractSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


