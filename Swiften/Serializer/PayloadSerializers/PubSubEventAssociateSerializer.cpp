/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventAssociateSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubEventAssociateSerializer::PubSubEventAssociateSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventAssociateSerializer::~PubSubEventAssociateSerializer() {
}

std::string PubSubEventAssociateSerializer::serializePayload(boost::shared_ptr<PubSubEventAssociate> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("associate", "http://jabber.org/protocol/pubsub#event");
	element.setAttribute("node", payload->getNode());
	return element.serialize();
}


