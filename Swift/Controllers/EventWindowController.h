#pragma once

#include "Swift/Controllers/UIInterfaces/EventWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/EventWindow.h"
#include "Swift/Controllers/EventController.h"


namespace Swift {

	class EventWindowController {
		public:
			EventWindowController(EventController* eventController, EventWindowFactory* windowFactory);
			~EventWindowController();
		private:
			void handleEventQueueEventAdded(boost::shared_ptr<StanzaEvent> event);
			void handleEventConcluded(boost::shared_ptr<StanzaEvent> event);

			EventController* eventController_;
			EventWindowFactory* windowFactory_;
			EventWindow* window_;
	};

}
