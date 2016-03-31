/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventCollectionSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventAssociateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventDisassociateSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventCollectionSerializer::PubSubEventCollectionSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventCollectionSerializer::~PubSubEventCollectionSerializer() {
}

std::string PubSubEventCollectionSerializer::serializePayload(boost::shared_ptr<PubSubEventCollection> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("collection", "http://jabber.org/protocol/pubsub#event");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    element.addNode(boost::make_shared<XMLRawTextNode>(PubSubEventDisassociateSerializer(serializers).serialize(payload->getDisassociate())));
    element.addNode(boost::make_shared<XMLRawTextNode>(PubSubEventAssociateSerializer(serializers).serialize(payload->getAssociate())));
    return element.serialize();
}


