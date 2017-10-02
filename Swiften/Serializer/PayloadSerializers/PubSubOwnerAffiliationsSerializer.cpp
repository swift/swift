/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationsSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerAffiliationsSerializer::PubSubOwnerAffiliationsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerAffiliationsSerializer::~PubSubOwnerAffiliationsSerializer() {
}

std::string PubSubOwnerAffiliationsSerializer::serializePayload(std::shared_ptr<PubSubOwnerAffiliations> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("affiliations", "http://jabber.org/protocol/pubsub#owner");
    element.setAttribute("node", payload->getNode());
    for (const auto& item : payload->getAffiliations()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubOwnerAffiliationSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


