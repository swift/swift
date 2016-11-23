/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionsSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubSubscriptionsSerializer::PubSubSubscriptionsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubSubscriptionsSerializer::~PubSubSubscriptionsSerializer() {
}

std::string PubSubSubscriptionsSerializer::serializePayload(std::shared_ptr<PubSubSubscriptions> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscriptions", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    for (const auto& item : payload->getSubscriptions()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubSubscriptionSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


