/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubAffiliationSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubAffiliationsSerializer::PubSubAffiliationsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubAffiliationsSerializer::~PubSubAffiliationsSerializer() {
}

std::string PubSubAffiliationsSerializer::serializePayload(boost::shared_ptr<PubSubAffiliations> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("affiliations", "http://jabber.org/protocol/pubsub");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    foreach(boost::shared_ptr<PubSubAffiliation> item, payload->getAffiliations()) {
        element.addNode(boost::make_shared<XMLRawTextNode>(PubSubAffiliationSerializer(serializers).serialize(item)));
    }
    return element.serialize();
}


