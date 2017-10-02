/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/UnitTest/MockMUC.h>

namespace Swift {

MockMUC::MockMUC(const JID &muc)
: ownMUCJID(muc)
, newSubjectSet_("")
{
}

MockMUC::~MockMUC() {
}

void MockMUC::insertOccupant(const MUCOccupant& occupant)
{
    occupants_.insert(std::make_pair(occupant.getNick(), occupant));
    onOccupantJoined(occupant);
}

const MUCOccupant& MockMUC::getOccupant(const std::string& nick) {
    return occupants_.find(nick)->second;
}

bool MockMUC::hasOccupant(const std::string& nick) {
    return occupants_.find(nick) != occupants_.end();
}

void MockMUC::changeAffiliation(const JID &jid, MUCOccupant::Affiliation newAffilation) {
    std::map<std::string, MUCOccupant>::iterator i = occupants_.find(jid.getResource());
    if (i != occupants_.end()) {
        const MUCOccupant old = i->second;
        i->second = MUCOccupant(old.getNick(), old.getRole(), newAffilation);
        onOccupantAffiliationChanged(i->first, newAffilation, old.getAffiliation());
    }
}

void MockMUC::changeOccupantRole(const JID &jid, MUCOccupant::Role newRole) {
    std::map<std::string, MUCOccupant>::iterator i = occupants_.find(jid.getResource());
    if (i != occupants_.end()) {
        const MUCOccupant old = i->second;
        i->second = MUCOccupant(old.getNick(), newRole, old.getAffiliation());
        onOccupantRoleChanged(i->first, i->second, old.getRole());
    }
}
void MockMUC::changeSubject(const std::string& newSubject) {
    newSubjectSet_ = newSubject;
}

}
