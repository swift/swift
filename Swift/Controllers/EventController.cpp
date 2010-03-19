#include "Swift/Controllers/EventController.h"

#include <boost/bind.hpp>
#include <algorithm>


namespace Swift {

EventController::EventController() {
}

void EventController::handleIncomingEvent(boost::shared_ptr<Event> sourceEvent) {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(sourceEvent);
	if (messageEvent && messageEvent->isReadable()) {
		events_.push_back(sourceEvent);
		messageEvent->onConclusion.connect(boost::bind(&EventController::handleEventConcluded, this, messageEvent));
		onEventQueueLengthChange(events_.size());
		onEventQueueEventAdded(sourceEvent);
	}
}

void EventController::handleEventConcluded(boost::shared_ptr<Event> event) {
	events_.erase(std::remove(events_.begin(), events_.end(), event), events_.end());
	onEventQueueLengthChange(events_.size());
}

}
