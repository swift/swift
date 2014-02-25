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
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/MAMQuerySerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>

using namespace Swift;

MAMQuerySerializer::MAMQuerySerializer(PayloadSerializerCollection* serializers) : serializers_(serializers) {
}

MAMQuerySerializer::~MAMQuerySerializer() {
}

std::string MAMQuerySerializer::serializePayload(boost::shared_ptr<MAMQuery> payload) const {
	if (!payload) {
		return "";
	}

	XMLElement element("query", "urn:xmpp:mam:0");

	if (payload->getQueryID()) {
		element.setAttribute("queryid", *payload->getQueryID());
	}

	if (payload->getForm()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(payload->getForm())));
	}

	if (payload->getResultSet()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(ResultSetSerializer().serialize(payload->getResultSet())));
	}

	return element.serialize();
}
