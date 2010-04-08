/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class AppearOffline : public RosterItemOperation {
	public:
		AppearOffline() {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact) {
				contact->setStatusShow(StatusShow::None);
			}
		}

};

}


