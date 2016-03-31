/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/StatusShow.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/RosterFilter.h>
#include <Swift/Controllers/Roster/RosterItem.h>

namespace Swift {

class OfflineRosterFilter : public RosterFilter {
    public:
        virtual ~OfflineRosterFilter() {}
        virtual bool operator() (RosterItem *item) const {
            ContactRosterItem *contactItem = dynamic_cast<ContactRosterItem*>(item);
            return contactItem && contactItem->getStatusShow() == StatusShow::None;
        }
};

}


