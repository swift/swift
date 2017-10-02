/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPubSubSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationsSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerConfigureSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDefaultSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDeleteSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerPurgeSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerSubscriptionsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerPubSubSerializer::PubSubOwnerPubSubSerializer(PayloadSerializerCollection* serializers) {
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerConfigureSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerSubscriptionsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerDefaultSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerPurgeSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerAffiliationsSerializer>(serializers));
    pubsubSerializers.push_back(std::make_shared<PubSubOwnerDeleteSerializer>(serializers));
}

PubSubOwnerPubSubSerializer::~PubSubOwnerPubSubSerializer() {
}

std::string PubSubOwnerPubSubSerializer::serializePayload(std::shared_ptr<PubSubOwnerPubSub> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("pubsub", "http://jabber.org/protocol/pubsub#owner");
    std::shared_ptr<PubSubOwnerPayload> p = payload->getPayload();
    for (const auto& serializer : pubsubSerializers) {
        if (serializer->canSerialize(p)) {
            element.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(p)));
        }
    }
    return element.serialize();
}


