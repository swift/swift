/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_SetAvatar_H
#define SWIFTEN_SetAvatar_H

#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class SetAvatar : public RosterItemOperation {
	public:
		SetAvatar(const JID& jid, const String& path, JID::CompareType compareType = JID::WithoutResource) : jid_(jid), path_(path), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(jid_, compareType_)) {
				contact->setAvatarPath(path_);
			}
		}
	
	private:
		JID jid_;
		String path_;
		JID::CompareType compareType_;
};

}
#endif

