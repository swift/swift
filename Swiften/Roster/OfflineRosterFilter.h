#ifndef SWIFTEN_OfflineRosterFilter_H
#define SWIFTEN_OfflineRosterFilter_H

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Roster/RosterFilter.h"
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
#endif



