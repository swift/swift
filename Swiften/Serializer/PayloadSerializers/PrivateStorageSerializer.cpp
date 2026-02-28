/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PrivateStorageSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

PrivateStorageSerializer::PrivateStorageSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

std::string PrivateStorageSerializer::serializePayload(std::shared_ptr<PrivateStorage> storage)    const {
    XMLElement storageElement("query", "jabber:iq:private");
    std::shared_ptr<Payload> payload = storage->getPayload();
    if (payload) {
        PayloadSerializer* serializer = serializers->getPayloadSerializer(payload);
        if (serializer) {
            storageElement.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(payload)));
        }
    }
    return storageElement.serialize();
}

}
