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


