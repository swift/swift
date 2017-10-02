/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventPurgeSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubEventPurgeSerializer::PubSubEventPurgeSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubEventPurgeSerializer::~PubSubEventPurgeSerializer() {
}

std::string PubSubEventPurgeSerializer::serializePayload(std::shared_ptr<PubSubEventPurge> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("purge", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


