/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/MUCUserPayloadSerializer.h>

#include <sstream>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/MUCItemSerializer.h>

namespace Swift {

MUCUserPayloadSerializer::MUCUserPayloadSerializer() : GenericPayloadSerializer<MUCUserPayload>() {
}

std::string MUCUserPayloadSerializer::serializePayload(boost::shared_ptr<MUCUserPayload> payload)  const {
	XMLElement mucElement("x", "http://jabber.org/protocol/muc#user");
	foreach (const MUCUserPayload::StatusCode statusCode, payload->getStatusCodes()) {
		boost::shared_ptr<XMLElement> statusElement(new XMLElement("status"));
		std::ostringstream code;
		code << statusCode.code;
		statusElement->setAttribute("code", code.str());
		mucElement.addNode(statusElement);
	}
	foreach (const MUCItem item, payload->getItems()) {
		mucElement.addNode(MUCItemSerializer::itemToElement(item));
	}
	return mucElement.serialize();
}




}
