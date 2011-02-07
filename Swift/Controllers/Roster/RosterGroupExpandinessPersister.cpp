/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "RosterGroupExpandinessPersister.h"

#include <boost/bind.hpp>
#include <vector>

#include "Swift/Controllers/Roster/GroupRosterItem.h"

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
		String displayName = group->getDisplayName();
		//collapsed_.erase(std::remove(collapsed_.begin(), collapsed_.end(), displayName), collapsed_.end());
		collapsed_.erase(displayName);
	} else {
		collapsed_.insert(group->getDisplayName());
	}
	save();
}

void RosterGroupExpandinessPersister::save() {
	String setting;
	foreach (const String& group, collapsed_) {
		if (!setting.isEmpty()) {
			setting += "\n";
		}
		setting += group;
	}
	settings_->storeString(SettingPath, setting);
}

void RosterGroupExpandinessPersister::load() {
	String saved = settings_->getStringSetting(SettingPath);
	std::vector<String> collapsed = saved.split('\n');
	collapsed_.insert(collapsed.begin(), collapsed.end());
}

const String RosterGroupExpandinessPersister::SettingPath = "GroupExpandiness";

}
