/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventConfigurationSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventSubscriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventPurgeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventCollectionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventDeleteSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemsSerializer.h>
#include <Swiften/Base/foreach.h>

using namespace Swift;

PubSubEventSerializer::PubSubEventSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
    pubsubSerializers.push_back(std::make_shared<PubSubEventSubscriptionSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubEventPurgeSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubEventCollectionSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubEventDeleteSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubEventItemsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubEventConfigurationSerializer>(serializers));
}

PubSubEventSerializer::~PubSubEventSerializer() {
}

std::string PubSubEventSerializer::serializePayload(std::shared_ptr<PubSubEvent> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("event", "http://jabber.org/protocol/pubsub#event");
    std::shared_ptr<PubSubEventPayload> p = payload->getPayload();
    foreach(std::shared_ptr<PayloadSerializer> serializer, pubsubSerializers) {
        if (serializer->canSerialize(p)) {
            element.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(p)));
        }
    }
    return element.serialize();
}


