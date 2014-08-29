/*
 * Copyright (c) 2014 Remko Tronçon and Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma clang diagnostic ignored "-Wunused-private-field"

#include <Swiften/Serializer/PayloadSerializers/IsodeIQDelegationSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

using namespace Swift;

IsodeIQDelegationSerializer::IsodeIQDelegationSerializer(PayloadSerializerCollection* serializers) : serializers(serializers) {
}

IsodeIQDelegationSerializer::~IsodeIQDelegationSerializer() {
}

std::string IsodeIQDelegationSerializer::serializePayload(boost::shared_ptr<IsodeIQDelegation> payload) const {
	if (!payload) {
		return "";
	}
	XMLElement element("delegate", "http://isode.com/iq_delegation");
	element.addNode(boost::make_shared<XMLRawTextNode>(serializers->getPayloadSerializer(payload->getForward())->serialize(payload->getForward())));
	return element.serialize();
}


