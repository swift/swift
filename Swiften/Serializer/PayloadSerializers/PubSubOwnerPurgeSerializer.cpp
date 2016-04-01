/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPurgeSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubOwnerPurgeSerializer::PubSubOwnerPurgeSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerPurgeSerializer::~PubSubOwnerPurgeSerializer() {
}

std::string PubSubOwnerPurgeSerializer::serializePayload(std::shared_ptr<PubSubOwnerPurge> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("purge", "http://jabber.org/protocol/pubsub#owner");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


