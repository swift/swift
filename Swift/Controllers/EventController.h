#ifndef SWIFTEN_EventController_H
#define SWIFTEN_EventController_H


#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Events/StanzaEvent.h"
#include "Swiften/Events/MessageEvent.h"

namespace Swift {
	class EventController {
		public:
			EventController();
			void handleIncomingEvent(boost::shared_ptr<StanzaEvent> sourceEvent);
			boost::signal<void (int)> onEventQueueLengthChange;
			boost::signal<void (boost::shared_ptr<StanzaEvent>)> onEventQueueEventAdded;

		private:
			void handleEventConcluded(boost::shared_ptr<StanzaEvent> event);
			std::vector<boost::shared_ptr<StanzaEvent> > events_;
	};
}
#endif


