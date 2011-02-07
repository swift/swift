/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swift/Controllers/Roster/RosterItemOperation.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class SetPresence : public RosterItemOperation {
	public:
		SetPresence(Presence::ref presence, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, compareType == JID::WithoutResource ? presence->getFrom().toBare() : presence->getFrom()), presence_(presence), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(presence_->getFrom(), compareType_)) {
				contact->applyPresence(presence_->getFrom().getResource(), presence_);
			}
		}
	
	private:
		Presence::ref presence_;
		JID::CompareType compareType_;
};

}

