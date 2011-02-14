/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/MUCOwnerPayloadSerializer.h"

#include "Swiften/Serializer/PayloadSerializerCollection.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLRawTextNode.h"

namespace Swift {

MUCOwnerPayloadSerializer::MUCOwnerPayloadSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<MUCOwnerPayload>(), serializers(serializers) {
}

std::string MUCOwnerPayloadSerializer::serializePayload(boost::shared_ptr<MUCOwnerPayload> mucOwner)  const {
	XMLElement mucElement("query", "http://jabber.org/protocol/muc#owner");
	boost::shared_ptr<Payload> payload = mucOwner->getPayload();
	if (payload) {
		PayloadSerializer* serializer = serializers->getPayloadSerializer(payload);
		if (serializer) {
			mucElement.addNode(boost::shared_ptr<XMLRawTextNode>(new XMLRawTextNode(serializer->serialize(payload))));
		}
	}
	return mucElement.serialize();
}

}
