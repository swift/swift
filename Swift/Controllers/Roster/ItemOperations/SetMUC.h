/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>

namespace Swift {

class RosterItem;

class SetMUC : public RosterItemOperation {
    public:
        SetMUC(const JID& jid, const MUCOccupant::Role& role, const MUCOccupant::Affiliation& affiliation)
        : RosterItemOperation(true, jid), jid_(jid), mucRole_(role), mucAffiliation_(affiliation) {
        }

        virtual void operator() (RosterItem* item) const {
            ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
            if (contact && contact->getJID().equals(jid_, JID::WithResource)) {
                contact->setMUCRole(mucRole_);
                contact->setMUCAffiliation(mucAffiliation_);
            }
        }

    private:
        JID jid_;
        MUCOccupant::Role mucRole_;
        MUCOccupant::Affiliation mucAffiliation_;
};

}
