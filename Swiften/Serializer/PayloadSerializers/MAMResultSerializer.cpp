/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MAMResultSerializer.h>

using namespace Swift;

MAMResultSerializer::MAMResultSerializer(PayloadSerializerCollection* serializers) : serializers_(serializers) {
}

MAMResultSerializer::~MAMResultSerializer() {
}

std::string MAMResultSerializer::serializePayload(boost::shared_ptr<MAMResult> payload) const {
	if (!payload) {
		return "";
	}

	XMLElement element("result", "urn:xmpp:mam:0");

	element.setAttribute("id", payload->getID());

	if (payload->getQueryID()) {
		element.setAttribute("queryid", *payload->getQueryID());
	}

	element.addNode(boost::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(payload->getPayload())));

	return element.serialize();
}
