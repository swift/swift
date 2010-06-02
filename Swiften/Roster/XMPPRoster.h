/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_XMPPRoster_H
#define SWIFTEN_XMPPRoster_H

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/RosterItemPayload.h"

#include <map>
#include <vector>
#include "Swiften/Base/boost_signalslib.h"

namespace Swift {

struct XMPPRosterItem {
	JID jid;
	String name;
	std::vector<String> groups;
	RosterItemPayload::Subscription subscription;
};

class XMPPRoster {
	public:
		XMPPRoster() {};
		~XMPPRoster() {};

		void addContact(const JID& jid, const String& name, const std::vector<String>& groups, const RosterItemPayload::Subscription subscription);
		bool containsJID(const JID& jid); 
		void removeContact(const JID& jid);
		RosterItemPayload::Subscription getSubscriptionStateForJID(const JID& jid);
		const String& getNameForJID(const JID& jid);
		const std::vector<String>& getGroupsForJID(const JID& jid);

		boost::signal<void (const JID&)> onJIDAdded;
		boost::signal<void (const JID&)> onJIDRemoved;
		boost::signal<void (const JID&, const String&, const std::vector<String>&)> onJIDUpdated;

	private:
		//std::map<JID, std::pair<String, std::vector<String> > > entries_;
		std::map<JID, XMPPRosterItem> entries_;
};
}

#endif

