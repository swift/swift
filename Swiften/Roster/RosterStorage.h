/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>

namespace Swift {
	class SWIFTEN_API RosterStorage {
		public:
			virtual ~RosterStorage();

			virtual boost::shared_ptr<RosterPayload> getRoster() const = 0;
			virtual void setRoster(boost::shared_ptr<RosterPayload>) = 0;
	};
}
