/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubUnsubscribeSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubUnsubscribeSerializer::PubSubUnsubscribeSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubUnsubscribeSerializer::~PubSubUnsubscribeSerializer() {
}

std::string PubSubUnsubscribeSerializer::serializePayload(boost::shared_ptr<PubSubUnsubscribe> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("unsubscribe", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    element.setAttribute("jid", payload->getJID());
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    return element.serialize();
}


