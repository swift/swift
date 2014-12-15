/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDeleteSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerRedirectSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerDeleteSerializer::PubSubOwnerDeleteSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerDeleteSerializer::~PubSubOwnerDeleteSerializer() {
}

std::string PubSubOwnerDeleteSerializer::serializePayload(boost::shared_ptr<PubSubOwnerDelete> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("delete", "http://jabber.org/protocol/pubsub#owner");
	element.setAttribute("node", payload->getNode());
	element.addNode(boost::make_shared<XMLRawTextNode>(PubSubOwnerRedirectSerializer(serializers).serialize(payload->getRedirect())));
	return element.serialize();
}


