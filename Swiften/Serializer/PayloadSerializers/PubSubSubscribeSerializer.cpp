/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubSubscribeSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubSubscribeSerializer::PubSubSubscribeSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubSubscribeSerializer::~PubSubSubscribeSerializer() {
}

std::string PubSubSubscribeSerializer::serializePayload(boost::shared_ptr<PubSubSubscribe> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("subscribe", "http://jabber.org/protocol/pubsub");
	if (payload->getNode()) {
		element.setAttribute("node", *payload->getNode());
	}
	element.setAttribute("jid", payload->getJID());
	return element.serialize();
}


