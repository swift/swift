/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeOptionsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubSubscriptionSerializer::PubSubSubscriptionSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubSubscriptionSerializer::~PubSubSubscriptionSerializer() {
}

std::string PubSubSubscriptionSerializer::serializePayload(std::shared_ptr<PubSubSubscription> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscription", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    element.setAttribute("jid", payload->getJID());
    element.addNode(std::make_shared<XMLRawTextNode>(PubSubSubscribeOptionsSerializer(serializers).serialize(payload->getOptions())));
    element.setAttribute("subscription", serializeSubscriptionType(payload->getSubscription()));
    return element.serialize();
}

std::string PubSubSubscriptionSerializer::serializeSubscriptionType(PubSubSubscription::SubscriptionType value) {
    switch (value) {
        case PubSubSubscription::None: return "none";
        case PubSubSubscription::Pending: return "pending";
        case PubSubSubscription::Subscribed: return "subscribed";
        case PubSubSubscription::Unconfigured: return "unconfigured";
    }
    assert(false);
    return "";
}
