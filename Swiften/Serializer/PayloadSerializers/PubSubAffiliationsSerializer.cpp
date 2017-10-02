/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationsSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubAffiliationsSerializer::PubSubAffiliationsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubAffiliationsSerializer::~PubSubAffiliationsSerializer() {
}

std::string PubSubAffiliationsSerializer::serializePayload(std::shared_ptr<PubSubAffiliations> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("affiliations", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    for (const auto& item : payload->getAffiliations()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubAffiliationSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


