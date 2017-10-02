/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */



#include <Swiften/Serializer/PayloadSerializers/PubSubEventCollectionSerializer.h>

#include <memory>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventAssociateSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventDisassociateSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventCollectionSerializer::PubSubEventCollectionSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventCollectionSerializer::~PubSubEventCollectionSerializer() {
}

std::string PubSubEventCollectionSerializer::serializePayload(std::shared_ptr<PubSubEventCollection> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("collection", "http://jabber.org/protocol/pubsub#event");
    if (payload->getNode()) {
        element.setAttribute("node", *payload->getNode());
    }
    element.addNode(std::make_shared<XMLRawTextNode>(PubSubEventDisassociateSerializer(serializers).serialize(payload->getDisassociate())));
    element.addNode(std::make_shared<XMLRawTextNode>(PubSubEventAssociateSerializer(serializers).serialize(payload->getAssociate())));
    return element.serialize();
}


