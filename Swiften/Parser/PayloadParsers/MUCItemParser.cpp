/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MUCItemParser.h>

#include <boost/lexical_cast.hpp>

#include <Swiften/Elements/MUCOccupant.h>

#include <cassert>
#include <iostream>

namespace Swift {

MUCItem MUCItemParser::itemFromTree(ParserElement::ref root) {
	MUCItem item;
	std::string affiliation = root->getAttributes().getAttribute("affiliation");
	std::string role = root->getAttributes().getAttribute("role");
	std::string nick = root->getAttributes().getAttribute("nick");
	std::string jid = root->getAttributes().getAttribute("jid");
	item.affiliation = parseAffiliation(affiliation);
	item.role = parseRole(role);
	if (!jid.empty()) {
		item.realJID = JID(jid);
	}
	if (!nick.empty()) {
		item.nick = nick;
	}
	std::string xmlns = root->getNamespace();
	std::string reason = root->getChild("reason", xmlns)->getText();
	std::string actor = root->getChild("actor", xmlns)->getAttributes().getAttribute("jid");
	if (!reason.empty()) {
		item.reason = reason;
	}
	if (!actor.empty()) {
		item.actor = JID(actor);
	}

	return item;
}

boost::optional<MUCOccupant::Role> MUCItemParser::parseRole(const std::string& roleString) {
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
	return boost::optional<MUCOccupant::Role>();
}

boost::optional<MUCOccupant::Affiliation> MUCItemParser::parseAffiliation(const std::string& affiliationString) {
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
	return boost::optional<MUCOccupant::Affiliation>();
}

}


