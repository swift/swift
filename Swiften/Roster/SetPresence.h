/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class SetPresence : public RosterItemOperation {
	public:
		SetPresence(boost::shared_ptr<Presence> presence, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, compareType == JID::WithoutResource ? presence->getFrom().toBare() : presence->getFrom()), presence_(presence), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(presence_->getFrom(), compareType_)) {
				contact->applyPresence(presence_->getFrom().getResource(), presence_);
			}
		}
	
	private:
		boost::shared_ptr<Presence> presence_;
		JID::CompareType compareType_;
};

}

