/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubItemsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubItemSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubItemsSerializer::PubSubItemsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubItemsSerializer::~PubSubItemsSerializer() {
}

std::string PubSubItemsSerializer::serializePayload(boost::shared_ptr<PubSubItems> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("items", "http://jabber.org/protocol/pubsub");
	element.setAttribute("node", payload->getNode());
	foreach(boost::shared_ptr<PubSubItem> item, payload->getItems()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(PubSubItemSerializer(serializers).serialize(item)));
	}
	if (payload->getMaximumItems()) {
		element.setAttribute("max_items", boost::lexical_cast<std::string>(*payload->getMaximumItems()));
	}
	if (payload->getSubscriptionID()) {
		element.setAttribute("subid", *payload->getSubscriptionID());
	}
	return element.serialize();
}


