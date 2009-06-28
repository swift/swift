#ifndef SWIFTEN_EventController_H
#define SWIFTEN_EventController_H


#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Events/MessageEvent.h"

namespace Swift {
	class EventController {
		public:
			void handleIncomingEvent(boost::shared_ptr<MessageEvent> event);
			boost::signal<void (int)> onEventQueueLengthChange;
			boost::signal<void (boost::shared_ptr<MessageEvent>)> onEventQueueEventAdded;

		private:
			void handleEventRead(boost::shared_ptr<MessageEvent> event);
			std::vector<boost::shared_ptr<MessageEvent> > events_;
	};
}
#endif


