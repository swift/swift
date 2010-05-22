/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h"

#include <boost/lexical_cast.hpp>

#include "Swiften/MUC/MUCOccupant.h"

#include <cassert>
#include <iostream>

namespace Swift {

MUCUserPayloadParser::MUCUserPayloadParser() : level(TopLevel) {
}

void MUCUserPayloadParser::handleStartElement(const String& element, const String&, const AttributeMap& attributes) {
	if (level == ItemLevel) {
		if (element == "item") {
			MUCUserPayload::Item item;
			String affiliation = attributes.getAttribute("affiliation");
			String role = attributes.getAttribute("role");
			String nick = attributes.getAttribute("nick");
			String jid = attributes.getAttribute("jid");
			item.affiliation = parseAffiliation(affiliation);
			item.role = parseRole(role);
			if (!jid.isEmpty()) {
				item.realJID = JID(jid);
			}
			if (!nick.isEmpty()) {
				item.nick = nick;
			}
			getPayloadInternal()->addItem(item);
		} else if (element == "status") {
			MUCUserPayload::StatusCode status;
			try {
				status.code = boost::lexical_cast<int>(attributes.getAttribute("code").getUTF8Data());
				getPayloadInternal()->addStatusCode(status);
			} catch (boost::bad_lexical_cast&) {
			}
		}
	}
	++level;
}

MUCOccupant::Role MUCUserPayloadParser::parseRole(const String& roleString) const {
	if (roleString == "moderator") {
		return MUCOccupant::Moderator;
	}
	if (roleString == "participant") {
		return MUCOccupant::Participant;
	}
	if (roleString == "visitor") {
		return MUCOccupant::Visitor;
	}
	if (roleString == "none") {
		return MUCOccupant::NoRole;
	}
	return MUCOccupant::NoRole;
}

MUCOccupant::Affiliation MUCUserPayloadParser::parseAffiliation(const String& affiliationString) const {
	if (affiliationString == "owner") {
		return MUCOccupant::Owner;
	}
	if (affiliationString == "admin") {
		return MUCOccupant::Admin;
	}
	if (affiliationString == "member") {
		return MUCOccupant::Member;
	}
	if (affiliationString == "outcast") {
		return MUCOccupant::Outcast;
	}
	if (affiliationString == "none") {
		return MUCOccupant::NoAffiliation;
	}
	return MUCOccupant::NoAffiliation;
}


void MUCUserPayloadParser::handleEndElement(const String& /*element*/, const String&) {
	--level;
}

void MUCUserPayloadParser::handleCharacterData(const String& /*data*/) {

}

}
