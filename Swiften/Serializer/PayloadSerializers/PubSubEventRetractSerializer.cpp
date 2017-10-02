/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventRetractSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubEventRetractSerializer::PubSubEventRetractSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubEventRetractSerializer::~PubSubEventRetractSerializer() {
}

std::string PubSubEventRetractSerializer::serializePayload(std::shared_ptr<PubSubEventRetract> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("retract", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("id", payload->getID());
    return element.serialize();
}


