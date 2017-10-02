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

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>

namespace Swift {

class RosterItem;

class SetAvailableFeatures : public RosterItemOperation {
    public:
        SetAvailableFeatures(const JID& jid, const std::set<ContactRosterItem::Feature>& availableFeatures, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), jid_(jid), availableFeatures_(availableFeatures), compareType_(compareType) {
        }

        virtual void operator() (RosterItem* item) const {
            ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
            if (contact && contact->getJID().equals(jid_, compareType_)) {
                contact->setSupportedFeatures(availableFeatures_);
            }
        }

    private:
        JID jid_;
        std::set<ContactRosterItem::Feature> availableFeatures_;
        JID::CompareType compareType_;
};

}
