/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/ContactProvider.h>

namespace Swift {

class PresenceOracle;
class AvatarManager;
class XMPPRoster;

class ContactsFromXMPPRoster : public ContactProvider {
	public:
		ContactsFromXMPPRoster(XMPPRoster* roster, AvatarManager* avatarManager, PresenceOracle* presenceOracle);
		virtual ~ContactsFromXMPPRoster();

		virtual std::vector<Contact::ref> getContacts();
	private:
		XMPPRoster* roster_;
		AvatarManager* avatarManager_;
		PresenceOracle* presenceOracle_;
};

}
