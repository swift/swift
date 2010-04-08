/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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

