/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/JID/JID.h>

namespace Swift {
struct MUCItem {
    MUCItem() {}
    MUCItem(MUCOccupant::Affiliation affiliation, const JID& jid, MUCOccupant::Role role) : realJID(jid), affiliation(affiliation), role(role) {}
    MUCItem(MUCOccupant::Affiliation affiliation, MUCOccupant::Role role) : affiliation(affiliation), role(role) {}
    boost::optional<JID> realJID;
    boost::optional<std::string> nick;
    boost::optional<MUCOccupant::Affiliation> affiliation;
    boost::optional<MUCOccupant::Role> role;
    boost::optional<JID> actor;
    boost::optional<std::string> reason;
};
}
