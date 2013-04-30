/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/Controllers/ContactsFromXMPPRoster.h>

#include <Swiften/Base/foreach.h>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/XMPPRosterItem.h>

namespace Swift {

ContactsFromXMPPRoster::ContactsFromXMPPRoster(XMPPRoster* roster, AvatarManager* avatarManager, PresenceOracle* presenceOracle) : roster_(roster), avatarManager_(avatarManager), presenceOracle_(presenceOracle) {
}

ContactsFromXMPPRoster::~ContactsFromXMPPRoster() {
}

std::vector<Contact> ContactsFromXMPPRoster::getContacts() {
	std::vector<Contact> results;
	std::vector<XMPPRosterItem> rosterItems = roster_->getItems();
	foreach(const XMPPRosterItem& rosterItem, rosterItems) {
		Contact contact(rosterItem.getName().empty() ? rosterItem.getJID().toString() : rosterItem.getName(), rosterItem.getJID(), StatusShow::None,"");
		contact.statusType = presenceOracle_->getHighestPriorityPresence(contact.jid) ? presenceOracle_->getHighestPriorityPresence(contact.jid)->getShow() : StatusShow::None;
		contact.avatarPath = avatarManager_->getAvatarPath(contact.jid);
		results.push_back(contact);
	}
	return results;
}

}
