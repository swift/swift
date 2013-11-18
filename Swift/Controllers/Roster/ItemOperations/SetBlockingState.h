/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>
#include <Swift/Controllers/Roster/ContactRosterItem.h>

namespace Swift {

class RosterItem;

class SetBlockingState : public RosterItemOperation {
	public:
		SetBlockingState(const JID& jid, ContactRosterItem::BlockState state, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), jid_(jid), state_(state), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(jid_, compareType_)) {
				contact->setBlockState(state_);
			}
		}

	private:
		JID jid_;
		ContactRosterItem::BlockState state_;
		JID::CompareType compareType_;
};

}
