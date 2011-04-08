/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/RosterPayload.h>

namespace Swift {
	class RosterStorage {
		public:
			virtual ~RosterStorage();

			virtual boost::shared_ptr<RosterPayload> getRoster() const = 0;
			virtual void setRoster(boost::shared_ptr<RosterPayload>) = 0;
	};
}
