/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/XMPPEvents/StanzaEvent.h"

namespace Swift {
	class EventWindow {
		public:
			EventWindow(bool candelete = true) : canDelete_(candelete) {}

			bool canDelete() const {
				return canDelete_;
			}

			virtual ~EventWindow() {};	
			virtual void addEvent(boost::shared_ptr<StanzaEvent> event, bool active) = 0;
			virtual void removeEvent(boost::shared_ptr<StanzaEvent> event) = 0;

		private:
			bool canDelete_;
	};
}
