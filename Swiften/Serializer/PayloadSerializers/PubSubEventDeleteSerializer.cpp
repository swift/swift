/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Serializer/PayloadSerializers/PubSubEventDeleteSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventRedirectSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventDeleteSerializer::PubSubEventDeleteSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventDeleteSerializer::~PubSubEventDeleteSerializer() {
}

std::string PubSubEventDeleteSerializer::serializePayload(std::shared_ptr<PubSubEventDelete> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("delete", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    element.addNode(std::make_shared<XMLRawTextNode>(PubSubEventRedirectSerializer(serializers).serialize(payload->getRedirects())));
    return element.serialize();
}


