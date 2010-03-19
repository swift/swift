#include "Swift/Controllers/EventWindowController.h"

#include <boost/bind.hpp>

namespace Swift {

EventWindowController::EventWindowController(EventController* eventController, EventWindowFactory* windowFactory) {
	eventController_ = eventController;
	windowFactory_ = windowFactory;
	window_ = windowFactory_->createEventWindow();
	eventController_->onEventQueueEventAdded.connect(boost::bind(&EventWindowController::handleEventQueueEventAdded, this, _1));
}

void EventWindowController::handleEventQueueEventAdded(boost::shared_ptr<Event> event) {
	window_->addEvent(event, true);
}

}
