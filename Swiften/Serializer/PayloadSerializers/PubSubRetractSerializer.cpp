/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubRetractSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubRetractSerializer::PubSubRetractSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubRetractSerializer::~PubSubRetractSerializer() {
}

std::string PubSubRetractSerializer::serializePayload(std::shared_ptr<PubSubRetract> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("retract", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    foreach(std::shared_ptr<PubSubItem> item, payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
    }
    element.setAttribute("notify", payload->isNotify() ? "true" : "false");
    return element.serialize();
}


