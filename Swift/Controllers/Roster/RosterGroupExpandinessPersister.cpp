/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Roster/RosterGroupExpandinessPersister.h>

#include <boost/bind.hpp>
#include <vector>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/String.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/SettingConstants.h>

namespace Swift {

RosterGroupExpandinessPersister::RosterGroupExpandinessPersister(Roster* roster, SettingsProvider* settings) : roster_(roster), settings_(settings) {
	load();
	roster_->onGroupAdded.connect(boost::bind(&RosterGroupExpandinessPersister::handleGroupAdded, this, _1));
}

void RosterGroupExpandinessPersister::handleGroupAdded(GroupRosterItem* group) {
	if (collapsed_.find(group->getDisplayName()) != collapsed_.end()) {
		group->setExpanded(false);
	} else {
		group->setExpanded(true);
	}
	group->onExpandedChanged.connect(boost::bind(&RosterGroupExpandinessPersister::handleExpandedChanged, this, group, _1));
}

void RosterGroupExpandinessPersister::handleExpandedChanged(GroupRosterItem* group, bool expanded) {
	if (expanded) {
		std::string displayName = group->getDisplayName();
		//collapsed_.erase(std::remove(collapsed_.begin(), collapsed_.end(), displayName), collapsed_.end());
		collapsed_.erase(displayName);
	} else {
		collapsed_.insert(group->getDisplayName());
	}
	save();
}

void RosterGroupExpandinessPersister::save() {
	std::string setting;
	foreach (const std::string& group, collapsed_) {
		if (!setting.empty()) {
			setting += "\n";
		}
		setting += group;
	}
	settings_->storeSetting(SettingConstants::EXPANDED_ROSTER_GROUPS, setting);
}

void RosterGroupExpandinessPersister::load() {
	std::string saved = settings_->getSetting(SettingConstants::EXPANDED_ROSTER_GROUPS);
	std::vector<std::string> collapsed = String::split(saved, '\n');
	collapsed_.insert(collapsed.begin(), collapsed.end());
}



}
