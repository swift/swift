/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCOwnerPayloadSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {

MUCOwnerPayloadSerializer::MUCOwnerPayloadSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<MUCOwnerPayload>(), serializers(serializers) {
}

std::string MUCOwnerPayloadSerializer::serializePayload(std::shared_ptr<MUCOwnerPayload> mucOwner)  const {
    XMLElement mucElement("query", "http://jabber.org/protocol/muc#owner");
    std::shared_ptr<Payload> payload = mucOwner->getPayload();
    if (payload) {
        PayloadSerializer* serializer = serializers->getPayloadSerializer(payload);
        if (serializer) {
            mucElement.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(payload)));
        }
    }
    return mucElement.serialize();
}

}
