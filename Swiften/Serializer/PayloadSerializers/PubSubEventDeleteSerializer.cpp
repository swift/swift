/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventDeleteSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubEventRedirectSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubEventDeleteSerializer::PubSubEventDeleteSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventDeleteSerializer::~PubSubEventDeleteSerializer() {
}

std::string PubSubEventDeleteSerializer::serializePayload(boost::shared_ptr<PubSubEventDelete> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("delete", "http://jabber.org/protocol/pubsub#event");
	element.setAttribute("node", payload->getNode());
	element.addNode(boost::make_shared<XMLRawTextNode>(PubSubEventRedirectSerializer(serializers).serialize(payload->getRedirects())));
	return element.serialize();
}


