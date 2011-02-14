/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h"

#include <boost/lexical_cast.hpp>

#include "Swiften/Elements/MUCOccupant.h"

#include <cassert>
#include <iostream>

namespace Swift {

MUCUserPayloadParser::MUCUserPayloadParser() : level(TopLevel) {
}

void MUCUserPayloadParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
	if (level == ItemLevel) {
		if (element == "item") {
			MUCUserPayload::Item item;
			std::string affiliation = attributes.getAttribute("affiliation");
			std::string role = attributes.getAttribute("role");
			std::string nick = attributes.getAttribute("nick");
			std::string jid = attributes.getAttribute("jid");
			item.affiliation = parseAffiliation(affiliation);
			item.role = parseRole(role);
			if (!jid.empty()) {
				item.realJID = JID(jid);
			}
			if (!nick.empty()) {
				item.nick = nick;
			}
			getPayloadInternal()->addItem(item);
		} else if (element == "status") {
			MUCUserPayload::StatusCode status;
			try {
				status.code = boost::lexical_cast<int>(attributes.getAttribute("code").c_str());
				getPayloadInternal()->addStatusCode(status);
			} catch (boost::bad_lexical_cast&) {
			}
		}
	}
	++level;
}

MUCOccupant::Role MUCUserPayloadParser::parseRole(const std::string& roleString) const {
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

MUCOccupant::Affiliation MUCUserPayloadParser::parseAffiliation(const std::string& affiliationString) const {
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


void MUCUserPayloadParser::handleEndElement(const std::string& /*element*/, const std::string&) {
	--level;
}

void MUCUserPayloadParser::handleCharacterData(const std::string& /*data*/) {

}

}
