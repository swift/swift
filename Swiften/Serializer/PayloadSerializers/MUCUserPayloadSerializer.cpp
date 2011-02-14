/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/MUCUserPayloadSerializer.h"

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"


namespace Swift {

MUCUserPayloadSerializer::MUCUserPayloadSerializer() : GenericPayloadSerializer<MUCUserPayload>() {
}

std::string MUCUserPayloadSerializer::serializePayload(boost::shared_ptr<MUCUserPayload> payload)  const {
	XMLElement mucElement("x", "http://jabber.org/protocol/muc");
	foreach (const MUCUserPayload::StatusCode statusCode, payload->getStatusCodes()) {
		boost::shared_ptr<XMLElement> statusElement(new XMLElement("status"));
		std::ostringstream code;
		code << statusCode.code;
		statusElement->setAttribute("code", code.str());
		mucElement.addNode(statusElement);
	}
	foreach (const MUCUserPayload::Item item, payload->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
		itemElement->setAttribute("affiliation", affiliationToString(item.affiliation));
		itemElement->setAttribute("role", roleToString(item.role));
		if (item.realJID) {
			itemElement->setAttribute("jid", item.realJID.get());
		}
		if (item.nick) {
			itemElement->setAttribute("nick", item.nick.get());
		}
		mucElement.addNode(itemElement);
	}
	return mucElement.serialize();
}

std::string MUCUserPayloadSerializer::affiliationToString(MUCOccupant::Affiliation affiliation) const {
	std::string result;
	switch (affiliation) {
	case MUCOccupant::Owner: result = "owner"; break;
	case MUCOccupant::Admin: result = "admin"; break;
	case MUCOccupant::Member: result = "member"; break;
	case MUCOccupant::Outcast: result = "outcast"; break;
	case MUCOccupant::NoAffiliation: result = "none"; break;
	default: assert(false);
	}
	return result;
}

std::string MUCUserPayloadSerializer::roleToString(MUCOccupant::Role role) const {
	std::string result;
	switch (role) {
	case MUCOccupant::Moderator: result = "moderator"; break;
	case MUCOccupant::NoRole: result = "none"; break;
	case MUCOccupant::Participant: result = "participant"; break;
	case MUCOccupant::Visitor: result = "visitor"; break;
	default: assert(false);
	}
	return result;

}


}
