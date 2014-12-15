/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerConfigureSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerConfigureSerializer::PubSubOwnerConfigureSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerConfigureSerializer::~PubSubOwnerConfigureSerializer() {
}

std::string PubSubOwnerConfigureSerializer::serializePayload(boost::shared_ptr<PubSubOwnerConfigure> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("configure", "http://jabber.org/protocol/pubsub#owner");
	if (payload->getNode()) {
		element.setAttribute("node", *payload->getNode());
	}
	element.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
	return element.serialize();
}


