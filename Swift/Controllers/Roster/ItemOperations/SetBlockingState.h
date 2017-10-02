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

class SetBlockingState : public RosterItemOperation {
    public:
        SetBlockingState(const JID& jid, ContactRosterItem::BlockState state, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(!jid.getNode().empty(), jid), jid_(jid), state_(state), compareType_(compareType) {
            if (state_ == ContactRosterItem::IsBlocked && jid.getNode().empty()) {
                state_ = ContactRosterItem::IsDomainBlocked;
            }
        }

        virtual void operator() (RosterItem* item) const {
            ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
            if (jid_.getNode().empty()) {
                if (contact && contact->getJID().getDomain() == jid_.getDomain()) {
                    contact->setBlockState(state_);
                }
            } else {
                if (contact && contact->getJID().equals(jid_, compareType_)) {
                    contact->setBlockState(state_);
                }
            }
        }

    private:
        JID jid_;
        ContactRosterItem::BlockState state_;
        JID::CompareType compareType_;
};

}
