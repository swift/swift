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
#include <Swiften/Serializer/PayloadSerializers/MAMArchivedSerializer.h>

using namespace Swift;

MAMArchivedSerializer::MAMArchivedSerializer(PayloadSerializerCollection* serializers) : serializers_(serializers) {
}

MAMArchivedSerializer::~MAMArchivedSerializer() {
}

std::string MAMArchivedSerializer::serializePayload(boost::shared_ptr<MAMArchived> payload) const {
	if (!payload) {
		return "";
	}

	XMLElement element("archived", "urn:xmpp:mam:0");
	element.setAttribute("by", payload->getBy());
	element.setAttribute("id", payload->getID());

	return element.serialize();
}
