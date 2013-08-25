/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubEventDisassociateSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubEventDisassociateSerializer::PubSubEventDisassociateSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubEventDisassociateSerializer::~PubSubEventDisassociateSerializer() {
}

std::string PubSubEventDisassociateSerializer::serializePayload(boost::shared_ptr<PubSubEventDisassociate> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("disassociate", "http://jabber.org/protocol/pubsub#event");
	element.setAttribute("node", payload->getNode());
	return element.serialize();
}


