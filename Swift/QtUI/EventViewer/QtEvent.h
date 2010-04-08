/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QVariant>

#include "Swiften/Events/StanzaEvent.h"

namespace Swift {
	class QtEvent {
		public:
			QtEvent(boost::shared_ptr<StanzaEvent> event, bool active);
			QVariant data(int role);
			boost::shared_ptr<StanzaEvent> getEvent() { return event_; };
		private:
			QString text();
			boost::shared_ptr<StanzaEvent> event_;
			bool active_;
	};
}
