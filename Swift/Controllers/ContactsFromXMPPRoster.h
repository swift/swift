/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
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

		virtual std::vector<Contact> getContacts();
	private:
		XMPPRoster* roster_;
		AvatarManager* avatarManager_;
		PresenceOracle* presenceOracle_;
};

}
