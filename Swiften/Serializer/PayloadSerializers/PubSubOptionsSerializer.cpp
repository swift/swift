/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOptionsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOptionsSerializer::PubSubOptionsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOptionsSerializer::~PubSubOptionsSerializer() {
}

std::string PubSubOptionsSerializer::serializePayload(boost::shared_ptr<PubSubOptions> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("options", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    element.setAttribute("jid", payload->getJID());
    element.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    return element.serialize();
}


