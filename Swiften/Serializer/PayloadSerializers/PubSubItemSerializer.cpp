/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubItemSerializer::PubSubItemSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubItemSerializer::~PubSubItemSerializer() {
}

std::string PubSubItemSerializer::serializePayload(boost::shared_ptr<PubSubItem> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("item", "http://jabber.org/protocol/pubsub");
	foreach(boost::shared_ptr<Payload> item, payload->getData()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(serializers->getPayloadSerializer(item)->serialize(item)));
	}
	if (!payload->getID().empty()) {
		element.setAttribute("id", payload->getID());
	}
	return element.serialize();
}


