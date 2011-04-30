/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {

MUCOccupant::MUCOccupant(const std::string &nick, Role role, Affiliation affiliation) : nick_(nick), role_(role), affiliation_(affiliation) {
}

MUCOccupant::~MUCOccupant() {
}

MUCOccupant::MUCOccupant(const MUCOccupant& other) : nick_(other.getNick()), role_(other.getRole()), affiliation_(other.getAffiliation()), realJID_(other.getRealJID()) {

}

std::string MUCOccupant::getNick() const {
	return nick_;
}

MUCOccupant::Role MUCOccupant::getRole() const {
	return role_;
}

MUCOccupant::Affiliation MUCOccupant::getAffiliation() const {
	return affiliation_;
}

void MUCOccupant::setRealJID(const JID& realJID) {
	realJID_ = realJID;
}

void MUCOccupant::setNick(const std::string& nick) {
	nick_ = nick;
}


boost::optional<JID> MUCOccupant::getRealJID() const {
	return realJID_;
}

}
