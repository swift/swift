#include "Swift/Controllers/EventController.h"

#include <boost/bind.hpp>
#include <algorithm>

#include "Swiften/Events/MessageEvent.h"
#include "Swiften/Events/SubscriptionRequestEvent.h"

namespace Swift {

EventController::EventController() {
}

void EventController::handleIncomingEvent(boost::shared_ptr<StanzaEvent> sourceEvent) {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(sourceEvent);
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(sourceEvent);
	if ((messageEvent && messageEvent->isReadable()) || subscriptionEvent) {
		events_.push_back(sourceEvent);
		sourceEvent->onConclusion.connect(boost::bind(&EventController::handleEventConcluded, this, sourceEvent));
		onEventQueueLengthChange(events_.size());
		onEventQueueEventAdded(sourceEvent);
	}
}

void EventController::handleEventConcluded(boost::shared_ptr<StanzaEvent> event) {
	events_.erase(std::remove(events_.begin(), events_.end(), event), events_.end());
	onEventQueueLengthChange(events_.size());
}

}
