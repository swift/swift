/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventConfigurationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventConfigurationSerializer::PubSubEventConfigurationSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventConfigurationSerializer::~PubSubEventConfigurationSerializer() {
}

std::string PubSubEventConfigurationSerializer::serializePayload(std::shared_ptr<PubSubEventConfiguration> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("configuration", "http://jabber.org/protocol/pubsub#event");
    element.setAttribute("node", payload->getNode());
    element.addNode(std::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
    return element.serialize();
}


