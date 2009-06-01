#ifndef SWIFTEN_RosterItemOperation_H
#define SWIFTEN_RosterItemOperation_H

#include "Swiften/Roster/RosterItem.h"

namespace Swift {

class RosterItemOperation {
	public:
		virtual ~RosterItemOperation() {}
		virtual void operator() (RosterItem*) const = 0;
};

}
#endif

