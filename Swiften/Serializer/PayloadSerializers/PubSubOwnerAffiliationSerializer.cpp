/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/PubSubOwnerAffiliationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>


#include <Swiften/Serializer/PayloadSerializerCollection.h>


using namespace Swift;

PubSubOwnerAffiliationSerializer::PubSubOwnerAffiliationSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

PubSubOwnerAffiliationSerializer::~PubSubOwnerAffiliationSerializer() {
}

std::string PubSubOwnerAffiliationSerializer::serializePayload(boost::shared_ptr<PubSubOwnerAffiliation> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("affiliation", "http://jabber.org/protocol/pubsub#owner");
	element.setAttribute("jid", payload->getJID());
	element.setAttribute("affiliation", serializeType(payload->getType()));
	return element.serialize();
}

std::string PubSubOwnerAffiliationSerializer::serializeType(PubSubOwnerAffiliation::Type value) {
	switch (value) {
		case PubSubOwnerAffiliation::None: return "none";
		case PubSubOwnerAffiliation::Member: return "member";
		case PubSubOwnerAffiliation::Outcast: return "outcast";
		case PubSubOwnerAffiliation::Owner: return "owner";
		case PubSubOwnerAffiliation::Publisher: return "publisher";
		case PubSubOwnerAffiliation::PublishOnly: return "publish-only";
	}
	assert(false);
	return "";
}
