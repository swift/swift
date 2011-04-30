/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <set>

#include <Swiften/Roster/XMPPRoster.h>

namespace Swift {
	class XMPPRosterImpl : public XMPPRoster {
		public:
			XMPPRosterImpl();

			void addContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, const RosterItemPayload::Subscription subscription);
			void removeContact(const JID& jid);
			void clear();

			bool containsJID(const JID& jid);
			RosterItemPayload::Subscription getSubscriptionStateForJID(const JID& jid);
			std::string getNameForJID(const JID& jid) const;
			std::vector<std::string> getGroupsForJID(const JID& jid);

			virtual std::vector<XMPPRosterItem> getItems() const;
			virtual boost::optional<XMPPRosterItem> getItem(const JID&) const;
			virtual std::set<std::string> getGroups() const;

		private:
			typedef std::map<JID, XMPPRosterItem> RosterMap;
			RosterMap entries_;
	};
}
