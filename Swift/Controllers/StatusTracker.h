/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Presence.h"

namespace Swift {

	class StatusTracker {
		public:
			StatusTracker();
			boost::shared_ptr<Presence> getNextPresence();
			void setRequestedPresence(boost::shared_ptr<Presence> presence);
			bool goAutoAway();
			bool goAutoUnAway();
		private:
			boost::shared_ptr<Presence> queuedPresence_;
			bool isAutoAway_;
	};
}
