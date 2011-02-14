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

class SetAvatar : public RosterItemOperation {
	public:
		SetAvatar(const JID& jid, const std::string& path, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), jid_(jid), path_(path), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(jid_, compareType_)) {
				contact->setAvatarPath(path_);
			}
		}
	
	private:
		JID jid_;
		std::string path_;
		JID::CompareType compareType_;
};

}
