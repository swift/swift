#pragma once

#include "boost/shared_ptr.hpp"
#include "Swiften/Events/StanzaEvent.h"

namespace Swift {
	class EventWindow {
		public:
			virtual ~EventWindow() {};	
			virtual void addEvent(boost::shared_ptr<StanzaEvent> event, bool active) = 0;
			virtual void removeEvent(boost::shared_ptr<StanzaEvent> event) = 0;
	};
}
