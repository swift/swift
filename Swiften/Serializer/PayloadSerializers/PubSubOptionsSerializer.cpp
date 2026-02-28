/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOptionsSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOptionsSerializer::PubSubOptionsSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubOptionsSerializer::~PubSubOptionsSerializer() {
}

std::string PubSubOptionsSerializer::serializePayload(std::shared_ptr<PubSubOptions> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("options", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    element.setAttribute("jid", payload->getJID());
    element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    return element.serialize();
}


