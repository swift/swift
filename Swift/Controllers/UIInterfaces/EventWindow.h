#pragma once

#include "boost/shared_ptr.hpp"
#include "Swiften/Events/Event.h"

namespace Swift {
	class EventWindow {
		public:
			virtual ~EventWindow() {};	
			virtual void addEvent(boost::shared_ptr<Event> event, bool active) = 0;
			virtual void removeEvent(boost::shared_ptr<Event> event) = 0;
	};
}
