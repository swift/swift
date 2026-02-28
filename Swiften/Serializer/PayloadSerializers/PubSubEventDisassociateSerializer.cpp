/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubEventDisassociateSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

using namespace Swift;

PubSubEventDisassociateSerializer::PubSubEventDisassociateSerializer(PayloadSerializerCollection* /*serializers*/) {
}

PubSubEventDisassociateSerializer::~PubSubEventDisassociateSerializer() {
}

std::string PubSubEventDisassociateSerializer::serializePayload(std::shared_ptr<PubSubEventDisassociate> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("disassociate", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    return element.serialize();
}


