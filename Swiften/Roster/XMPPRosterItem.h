/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once


#include <vector>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/RosterItemPayload.h>

namespace Swift {
	class XMPPRosterItem {
		public:
			XMPPRosterItem(const JID& jid, const std::string& name, const std::vector<std::string>& groups, RosterItemPayload::Subscription subscription) : jid(jid), name(name), groups(groups), subscription(subscription) {
			}

			const JID& getJID() const {
				return jid;
			}

			const std::string& getName() const {
				return name;
			}

			void setName(const std::string& name) {
				this->name = name;
			}

			const std::vector<std::string>& getGroups() const {
				return groups;
			}

			void setGroups(const std::vector<std::string>& groups) {
				this->groups = groups;
			}

			RosterItemPayload::Subscription getSubscription() const {
				return subscription;
			}

		private:
			JID jid;
			std::string name;
			std::vector<std::string> groups;
			RosterItemPayload::Subscription subscription;
	};
}

