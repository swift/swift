/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerDefaultSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerDefaultSerializer::PubSubOwnerDefaultSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerDefaultSerializer::~PubSubOwnerDefaultSerializer() {
}

std::string PubSubOwnerDefaultSerializer::serializePayload(boost::shared_ptr<PubSubOwnerDefault> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("default", "http://jabber.org/protocol/pubsub#owner");
	element.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getData())));
	return element.serialize();
}


