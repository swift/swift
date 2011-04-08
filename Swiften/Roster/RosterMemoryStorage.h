/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Roster/RosterStorage.h>

namespace Swift {
	class RosterMemoryStorage : public RosterStorage {
		public:
			RosterMemoryStorage();

			virtual boost::shared_ptr<RosterPayload> getRoster() const {
				return roster;
			}

			virtual void setRoster(boost::shared_ptr<RosterPayload>);

		private:
			boost::shared_ptr<RosterPayload> roster;
	};
}
