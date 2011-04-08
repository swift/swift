/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Roster/RosterFileStorage.h>

#include <boost/smart_ptr/make_shared.hpp>

namespace Swift {

RosterFileStorage::RosterFileStorage(const boost::filesystem::path& path) : path(path) {
}

// FIXME
void RosterFileStorage::setRoster(boost::shared_ptr<RosterPayload> r) {
	roster.reset();
	if (r) {
		roster = boost::make_shared<RosterPayload>(*r);
	}
}

}
