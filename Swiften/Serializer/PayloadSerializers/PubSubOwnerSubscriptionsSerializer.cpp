/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerSubscriptionsSerializer::PubSubOwnerSubscriptionsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerSubscriptionsSerializer::~PubSubOwnerSubscriptionsSerializer() {
}

std::string PubSubOwnerSubscriptionsSerializer::serializePayload(boost::shared_ptr<PubSubOwnerSubscriptions> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("subscriptions", "http://jabber.org/protocol/pubsub#owner");
    element.setAttribute("node", payload->getNode());
    foreach(boost::shared_ptr<PubSubOwnerSubscription> item, payload->getSubscriptions()) {
        element.addNode(boost::make_shared<XMLRawTextNode>(PubSubOwnerSubscriptionSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


