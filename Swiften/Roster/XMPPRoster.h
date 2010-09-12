/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/RosterItemPayload.h"

#include <map>
#include <vector>
#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	class XMPPRoster {
		public:
			XMPPRoster();

			void addContact(const JID& jid, const String& name, const std::vector<String>& groups, const RosterItemPayload::Subscription subscription);
			void removeContact(const JID& jid);
			void clear();

			bool containsJID(const JID& jid);
			RosterItemPayload::Subscription getSubscriptionStateForJID(const JID& jid);
			String getNameForJID(const JID& jid) const;
			const std::vector<String>& getGroupsForJID(const JID& jid);

			boost::signal<void (const JID&)> onJIDAdded;
			boost::signal<void (const JID&)> onJIDRemoved;
			boost::signal<void (const JID&, const String&, const std::vector<String>&)> onJIDUpdated;
			boost::signal<void ()> onRosterCleared;

		private:
			struct XMPPRosterItem {
				JID jid;
				String name;
				std::vector<String> groups;
				RosterItemPayload::Subscription subscription;
			};
			std::map<JID, XMPPRosterItem> entries_;
	};
}
