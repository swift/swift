/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubConfigureSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubCreateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubDefaultSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOptionsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubPublishSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubRetractSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubSubscriptionsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubUnsubscribeSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubSerializer::PubSubSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
    pubsubSerializers.push_back(std::make_shared<PubSubItemsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubCreateSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubPublishSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOptionsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubAffiliationsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubRetractSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubDefaultSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubSubscriptionsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubSubscribeSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubUnsubscribeSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubSubscriptionSerializer>(serializers));
}

PubSubSerializer::~PubSubSerializer() {
}

std::string PubSubSerializer::serializePayload(std::shared_ptr<PubSub> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("pubsub", "http://jabber.org/protocol/pubsub");
    std::shared_ptr<PubSubPayload> p = payload->getPayload();
    for (const auto& serializer : pubsubSerializers) {
        if (serializer->canSerialize(p)) {
            element.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(p)));
            if (std::shared_ptr<PubSubCreate> create = std::dynamic_pointer_cast<PubSubCreate>(p)) {
                element.addNode(std::make_shared<XMLRawTextNode>(std::make_shared<PubSubConfigureSerializer>(serializers)->serialize(create->getConfigure())));
            }
            if (std::shared_ptr<PubSubSubscribe> subscribe = std::dynamic_pointer_cast<PubSubSubscribe>(p)) {
                element.addNode(std::make_shared<XMLRawTextNode>(std::make_shared<PubSubConfigureSerializer>(serializers)->serialize(subscribe->getOptions())));
            }
        }
    }
    return element.serialize();
}


