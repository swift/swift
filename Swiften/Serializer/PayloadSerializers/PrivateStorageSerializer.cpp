/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/PrivateStorageSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {

PrivateStorageSerializer::PrivateStorageSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

std::string PrivateStorageSerializer::serializePayload(boost::shared_ptr<PrivateStorage> storage)	const {
	XMLElement storageElement("query", "jabber:iq:private");
	boost::shared_ptr<Payload> payload = storage->getPayload();
	if (payload) {
		PayloadSerializer* serializer = serializers->getPayloadSerializer(payload);
		if (serializer) {
			storageElement.addNode(boost::make_shared<XMLRawTextNode>(serializer->serialize(payload)));
		}
	}
	return storageElement.serialize();
}

}
