/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Roster/RosterItem.h"

namespace Swift {

class RosterFilter {
	public:
		virtual ~RosterFilter() {}
		virtual bool operator() (RosterItem* item) const = 0;
};

}


