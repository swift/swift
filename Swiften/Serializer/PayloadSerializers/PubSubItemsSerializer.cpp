/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PubSubItemsSerializer.h>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubItemsSerializer::PubSubItemsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubItemsSerializer::~PubSubItemsSerializer() {
}

std::string PubSubItemsSerializer::serializePayload(std::shared_ptr<PubSubItems> payload) const {
    if (!payload) {
        return "";
    }
    XMLElement element("items", "http://jabber.org/protocol/pubsub");
    if (payload->getNode().empty()) {
        SWIFT_LOG(warning) << "Serializing PubSubItems with empty node attribute";
    }
    element.setAttribute("node", payload->getNode());
    for (const auto& item : payload->getItems()) {
        element.addNode(std::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
    }
    if (payload->getMaximumItems()) {
        element.setAttribute("max_items", boost::lexical_cast<std::string>(*payload->getMaximumItems()));
    }
    if (payload->getSubscriptionID()) {
        element.setAttribute("subid", *payload->getSubscriptionID());
    }
    return element.serialize();
}


