/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	/**
	 * An event for regrouping a roster item.
	 * This doesn't need to cover all groups, so it's valid to have the
	 * contact in several groups that are neither removedGroups or addedGroups.
	 */
	class RegroupRosterItemUIEvent : public UIEvent {
		public:
			RegroupRosterItemUIEvent(const JID& jid, const std::vector<String>& addedGroups, const std::vector<String>& removedGroups) : jid_(jid), addedGroups_(addedGroups), removedGroups_(removedGroups) {}

			const JID& getJID() const {return jid_;}
			const std::vector<String>& getAddedGroups() const {return addedGroups_;}
			const std::vector<String>& getRemovedGroups() const {return removedGroups_;}

		private:
			JID jid_;
			std::vector<String> addedGroups_;
			std::vector<String> removedGroups_;
	};
}
