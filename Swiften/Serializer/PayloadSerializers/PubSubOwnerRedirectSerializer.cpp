/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerRedirectSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubOwnerRedirectSerializer::PubSubOwnerRedirectSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerRedirectSerializer::~PubSubOwnerRedirectSerializer() {
}

std::string PubSubOwnerRedirectSerializer::serializePayload(boost::shared_ptr<PubSubOwnerRedirect> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("redirect", "http://jabber.org/protocol/pubsub#owner");
	element.setAttribute("uri", payload->getURI());
	return element.serialize();
}


