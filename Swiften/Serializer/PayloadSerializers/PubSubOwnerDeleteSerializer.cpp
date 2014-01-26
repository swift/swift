/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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


