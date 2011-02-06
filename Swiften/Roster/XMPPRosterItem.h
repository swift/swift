/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once


#include <vector>

#include <Swiften/Base/String.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/RosterItemPayload.h>

namespace Swift {
	class XMPPRosterItem {
		public:
			XMPPRosterItem(const JID& jid, const String& name, const std::vector<String>& groups, RosterItemPayload::Subscription subscription) : jid(jid), name(name), groups(groups), subscription(subscription) {
			}

			const JID& getJID() const {
				return jid;
			}

			const String& getName() const {
				return name;
			}

			void setName(const String& name) {
				this->name = name;
			}

			const std::vector<String>& getGroups() const {
				return groups;
			}

			void setGroups(const std::vector<String>& groups) {
				this->groups = groups;
			}

			RosterItemPayload::Subscription getSubscription() const {
				return subscription;
			}

		private:
			JID jid;
			String name;
			std::vector<String> groups;
			RosterItemPayload::Subscription subscription;
	};
}

