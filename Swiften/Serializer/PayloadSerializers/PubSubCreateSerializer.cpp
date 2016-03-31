/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubCreateSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubCreateSerializer::PubSubCreateSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubCreateSerializer::~PubSubCreateSerializer() {
}

std::string PubSubCreateSerializer::serializePayload(boost::shared_ptr<PubSubCreate> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("create", "http://jabber.org/protocol/pubsub");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


