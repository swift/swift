/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/JID/JID.h>
namespace Swift {
struct MUCItem {
	MUCItem() {}
	boost::optional<JID> realJID;
	boost::optional<std::string> nick;
	boost::optional<MUCOccupant::Affiliation> affiliation;
	boost::optional<MUCOccupant::Role> role;
	boost::optional<JID> actor;
	boost::optional<std::string> reason;
};
}
