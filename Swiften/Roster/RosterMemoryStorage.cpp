/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Roster/RosterMemoryStorage.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

RosterMemoryStorage::RosterMemoryStorage() {
}

void RosterMemoryStorage::setRoster(boost::shared_ptr<RosterPayload> r) {
	roster.reset();
	if (r) {
		roster = boost::make_shared<RosterPayload>(*r);
	}
}

}
