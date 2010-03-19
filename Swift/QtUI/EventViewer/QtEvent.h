#pragma once

#include <boost/shared_ptr.hpp>

#include <QVariant>

#include "Swiften/Events/Event.h"

namespace Swift {
	class QtEvent {
		public:
			QtEvent(boost::shared_ptr<Event> event, bool active);
			QVariant data(int role);
			boost::shared_ptr<Event> getEvent() { return event_; };
		private:
			QString text();
			boost::shared_ptr<Event> event_;
			bool active_;
	};
}
