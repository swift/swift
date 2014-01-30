/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventRetractSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventItemsSerializer::PubSubEventItemsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventItemsSerializer::~PubSubEventItemsSerializer() {
}

std::string PubSubEventItemsSerializer::serializePayload(boost::shared_ptr<PubSubEventItems> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("items", "http://jabber.org/protocol/pubsub#event");
	element.setAttribute("node", payload->getNode());
	foreach(boost::shared_ptr<PubSubEventItem> item, payload->getItems()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(PubSubEventItemSerializer(serializers).serialize(item)));
	}
	foreach(boost::shared_ptr<PubSubEventRetract> item, payload->getRetracts()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(PubSubEventRetractSerializer(serializers).serialize(item)));
	}
	return element.serialize();
}


