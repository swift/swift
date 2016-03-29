/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QVariant>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
	class QtEvent {
		public:
			QtEvent(boost::shared_ptr<StanzaEvent> event, bool active);
			QVariant data(int role);
			boost::shared_ptr<StanzaEvent> getEvent() { return event_; }
			enum EventRoles {
				SenderRole = Qt::UserRole

			};

		private:
			QString text();
			QString sender();
			boost::shared_ptr<StanzaEvent> event_;
			bool active_;
	};
}
