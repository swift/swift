/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationsSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationSerializer.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

PubSubOwnerAffiliationsSerializer::PubSubOwnerAffiliationsSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerAffiliationsSerializer::~PubSubOwnerAffiliationsSerializer() {
}

std::string PubSubOwnerAffiliationsSerializer::serializePayload(boost::shared_ptr<PubSubOwnerAffiliations> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("affiliations", "http://jabber.org/protocol/pubsub#owner");
	element.setAttribute("node", payload->getNode());
	foreach(boost::shared_ptr<PubSubOwnerAffiliation> item, payload->getAffiliations()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(PubSubOwnerAffiliationSerializer(serializers).serialize(item)));
	}
	return element.serialize();
}


