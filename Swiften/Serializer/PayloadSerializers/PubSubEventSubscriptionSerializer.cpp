/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventSubscriptionSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/DateTime.h>

using namespace Swift;

PubSubEventSubscriptionSerializer::PubSubEventSubscriptionSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventSubscriptionSerializer::~PubSubEventSubscriptionSerializer() {
}

std::string PubSubEventSubscriptionSerializer::serializePayload(boost::shared_ptr<PubSubEventSubscription> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscription", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    element.setAttribute("jid", payload->getJID());
    element.setAttribute("subscription", serializeSubscriptionType(payload->getSubscription()));
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    element.setAttribute("expiry", dateTimeToString(payload->getExpiry()));
    return element.serialize();
}

std::string PubSubEventSubscriptionSerializer::serializeSubscriptionType(PubSubEventSubscription::SubscriptionType value) {
    switch (value) {
        case PubSubEventSubscription::None: return "none";
        case PubSubEventSubscription::Pending: return "pending";
        case PubSubEventSubscription::Subscribed: return "subscribed";
        case PubSubEventSubscription::Unconfigured: return "unconfigured";
    }
    assert(false);
    return "";
}
