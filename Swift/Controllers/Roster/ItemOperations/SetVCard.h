/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>

namespace Swift {

class RosterItem;

class SetVCard : public RosterItemOperation {
    public:
        SetVCard(const JID& jid, VCard::ref vcard, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), jid_(jid), vcard_(vcard), compareType_(compareType) {
        }

        virtual void operator() (RosterItem* item) const {
            ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
            if (contact && contact->getJID().equals(jid_, compareType_)) {
                contact->setVCard(vcard_);
            }
        }

    private:
        JID jid_;
        VCard::ref vcard_;
        JID::CompareType compareType_;
};

}
