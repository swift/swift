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
#include <Swiften/Serializer/PayloadSerializers/MAMFinSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ResultSetSerializer.h>

using namespace Swift;

MAMFinSerializer::MAMFinSerializer() {
}

MAMFinSerializer::~MAMFinSerializer() {
}

std::string MAMFinSerializer::serializePayload(boost::shared_ptr<MAMFin> payload) const {
	if (!payload) {
		return "";
	}

	XMLElement element("fin", "urn:xmpp:mam:0");

	if (payload->isComplete()) {
		element.setAttribute("complete", "true");
	}

	if (!payload->isStable()) {
		element.setAttribute("stable", "false");
	}

	if (payload->getQueryID()) {
		element.setAttribute("queryid", *payload->getQueryID());
	}

	if (payload->getResultSet()) {
		element.addNode(boost::make_shared<XMLRawTextNode>(ResultSetSerializer().serialize(payload->getResultSet())));
	}

	return element.serialize();
}
