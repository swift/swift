/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventAssociateSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubEventAssociateSerializer::PubSubEventAssociateSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubEventAssociateSerializer::~PubSubEventAssociateSerializer() {
}

std::string PubSubEventAssociateSerializer::serializePayload(std::shared_ptr<PubSubEventAssociate> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("associate", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


