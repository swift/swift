/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem/path.hpp>

#include <Swiften/Roster/RosterStorage.h>

namespace Swift {
	class RosterFileStorage : public RosterStorage {
		public:
			RosterFileStorage(const boost::filesystem::path& path);

			virtual boost::shared_ptr<RosterPayload> getRoster() const;
			virtual void setRoster(boost::shared_ptr<RosterPayload>);

		private:
			boost::filesystem::path path;
	};
}
