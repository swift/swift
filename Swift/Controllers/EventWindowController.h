#pragma once

#include "Swift/Controllers/UIInterfaces/EventWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/EventWindow.h"
#include "Swift/Controllers/EventController.h"


namespace Swift {

	class EventWindowController {
		public:
			EventWindowController(EventController* eventController, EventWindowFactory* windowFactory);
		private:
			void handleEventQueueEventAdded(boost::shared_ptr<Event> event);

			EventController* eventController_;
			EventWindowFactory* windowFactory_;
			EventWindow* window_;
	};

}
