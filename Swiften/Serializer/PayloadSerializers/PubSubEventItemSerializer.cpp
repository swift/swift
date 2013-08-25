/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventItemSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventItemSerializer::PubSubEventItemSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventItemSerializer::~PubSubEventItemSerializer() {
}

std::string PubSubEventItemSerializer::serializePayload(boost::shared_ptr<PubSubEventItem> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("item", "http://jabber.org/protocol/pubsub#event");
	if (payload->getNode()) {
		element.setAttribute("node", *payload->getNode());
	}
	if (payload->getPublisher()) {
		element.setAttribute("publisher", *payload->getPublisher());
	}
	foreach(boost::shared_ptr<Payload> item, payload->getData()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(serializers->getPayloadSerializer(item)->serialize(item)));
	}
	if (payload->getID()) {
		element.setAttribute("id", *payload->getID());
	}
	return element.serialize();
}


