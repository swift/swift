/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>

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
