#include "Swift/Controllers/EventController.h"

#include <boost/bind.hpp>
#include <algorithm>

namespace Swift {

void EventController::handleIncomingEvent(boost::shared_ptr<MessageEvent> event) {
	if (event->isReadable()) {
		events_.push_back(event);
		event->onRead.connect(boost::bind(&EventController::handleEventRead, this, event));
		onEventQueueLengthChange(events_.size());
		onEventQueueEventAdded(event);
	}
}

void EventController::handleEventRead(boost::shared_ptr<MessageEvent> event) {
	events_.erase(std::remove(events_.begin(), events_.end(), event), events_.end());
	onEventQueueLengthChange(events_.size());
}

}
