/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubRetractSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
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
    for (const auto& item : payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
    }
    if (payload->isNotify().is_initialized()) {
        element.setAttribute("notify", payload->isNotify().get() ? "true" : "false");
    }
    return element.serialize();
}


