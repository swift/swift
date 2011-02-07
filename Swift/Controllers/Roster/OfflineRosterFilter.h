/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/RosterItem.h"
#include "Swift/Controllers/Roster/RosterFilter.h"
#include "Swiften/Elements/StatusShow.h"

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


