/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventPurgeSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubEventPurgeSerializer::PubSubEventPurgeSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventPurgeSerializer::~PubSubEventPurgeSerializer() {
}

std::string PubSubEventPurgeSerializer::serializePayload(boost::shared_ptr<PubSubEventPurge> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("purge", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


