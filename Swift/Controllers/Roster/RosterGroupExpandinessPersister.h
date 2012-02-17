/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>
#include "Swift/Controllers/Roster/Roster.h"
#include "Swift/Controllers/Settings/SettingsProvider.h"

namespace Swift {
	class RosterGroupExpandinessPersister {
		public:
			RosterGroupExpandinessPersister(Roster* roster, SettingsProvider* settings);
		private:
			void handleExpandedChanged(GroupRosterItem* group, bool expanded);
			void handleGroupAdded(GroupRosterItem* group);
			void load();
			void save();
			std::set<std::string> collapsed_;
			Roster* roster_;
			SettingsProvider* settings_;
	};
}
