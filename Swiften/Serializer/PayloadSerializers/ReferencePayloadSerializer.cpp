/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ReferencePayloadSerializer.h>

#include <memory>

#include <Swiften/Base/Log.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {

ReferencePayloadSerializer::ReferencePayloadSerializer(PayloadSerializerCollection* payloadSerializers) : GenericPayloadSerializer<ReferencePayload>(), payloadSerializers_(payloadSerializers) {
}

std::string ReferencePayloadSerializer::serializePayload(ReferencePayload::ref reference)  const {
    XMLElement element("reference", "urn:xmpp:reference:0");

    auto type = reference->getType();
    if (type != ReferencePayload::Type::Unknown) {
        element.setAttribute("type", getTypeString(type));

        if (auto uri = reference->getUri()) {
            element.setAttribute("uri", *uri);
        }
        if (auto begin = reference->getBegin()) {
            element.setAttribute("begin", *begin);
        }
        if (auto end = reference->getEnd()) {
            element.setAttribute("end", *end);
        }
        if (auto anchor = reference->getAnchor()) {
            element.setAttribute("anchor", *anchor);
        }

        std::string serializedPayloads;
        for (const auto& payload : reference->getPayloads()) {
            if (auto serializer = payloadSerializers_->getPayloadSerializer(payload)) {
                element.addNode(std::make_shared<XMLRawTextNode>(serializer->serialize(payload)));
            }
            else {
                SWIFT_LOG(warning) << "Could not find serializer for " << typeid(*(payload.get())).name() << std::endl;
            }
        }
    }
    return element.serialize();
}

const std::string ReferencePayloadSerializer::getTypeString(const ReferencePayload::Type type) const {
    switch(type) {
        case ReferencePayload::Type::Data:    return "data";
        case ReferencePayload::Type::Mention: return "mention";
        case ReferencePayload::Type::PubSub:  return "pubsub";
        case ReferencePayload::Type::Unknown: return "unknown";
    }
    return "";
}

}
