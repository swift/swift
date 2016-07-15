/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionSerializer.h>

#include <cassert>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubOwnerSubscriptionSerializer::PubSubOwnerSubscriptionSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubOwnerSubscriptionSerializer::~PubSubOwnerSubscriptionSerializer() {
}

std::string PubSubOwnerSubscriptionSerializer::serializePayload(std::shared_ptr<PubSubOwnerSubscription> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscription", "http://jabber.org/protocol/pubsub#owner");
    element.setAttribute("jid", payload->getJID());
    element.setAttribute("subscription", serializeSubscriptionType(payload->getSubscription()));
    return element.serialize();
}

std::string PubSubOwnerSubscriptionSerializer::serializeSubscriptionType(PubSubOwnerSubscription::SubscriptionType value) {
    switch (value) {
        case PubSubOwnerSubscription::None: return "none";
        case PubSubOwnerSubscription::Pending: return "pending";
        case PubSubOwnerSubscription::Subscribed: return "subscribed";
        case PubSubOwnerSubscription::Unconfigured: return "unconfigured";
    }
    assert(false);
    return "";
}
