/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/XMPPEvents/EventController.h>

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/foreach.h>

#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

namespace Swift {

EventController::EventController() {
}

EventController::~EventController() {
	foreach(boost::shared_ptr<StanzaEvent> event, events_) {
		event->onConclusion.disconnect(boost::bind(&EventController::handleEventConcluded, this, event));
	}
}

void EventController::handleIncomingEvent(boost::shared_ptr<StanzaEvent> sourceEvent) {
	boost::shared_ptr<MessageEvent> messageEvent = boost::dynamic_pointer_cast<MessageEvent>(sourceEvent);
	boost::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(sourceEvent);
	boost::shared_ptr<ErrorEvent> errorEvent = boost::dynamic_pointer_cast<ErrorEvent>(sourceEvent);
	boost::shared_ptr<MUCInviteEvent> mucInviteEvent = boost::dynamic_pointer_cast<MUCInviteEvent>(sourceEvent);
	boost::shared_ptr<IncomingFileTransferEvent> incomingFileTransferEvent = boost::dynamic_pointer_cast<IncomingFileTransferEvent>(sourceEvent);

	/* If it's a duplicate subscription request, remove the previous request first */
	if (subscriptionEvent) {
		EventList existingEvents(events_);
		foreach(boost::shared_ptr<StanzaEvent> existingEvent, existingEvents) {
			boost::shared_ptr<SubscriptionRequestEvent> existingSubscriptionEvent = boost::dynamic_pointer_cast<SubscriptionRequestEvent>(existingEvent);
			if (existingSubscriptionEvent) {
				if (existingSubscriptionEvent->getJID() == subscriptionEvent->getJID()) {
					existingEvent->conclude();
				}
			}
		}
	}

	if ((messageEvent && messageEvent->isReadable()) || subscriptionEvent || errorEvent || mucInviteEvent || incomingFileTransferEvent) {
		events_.push_back(sourceEvent);
		sourceEvent->onConclusion.connect(boost::bind(&EventController::handleEventConcluded, this, sourceEvent));
		onEventQueueLengthChange(boost::numeric_cast<int>(events_.size()));
		onEventQueueEventAdded(sourceEvent);
		if (sourceEvent->getConcluded()) {
			handleEventConcluded(sourceEvent);
		}
	}
}

void EventController::handleEventConcluded(boost::shared_ptr<StanzaEvent> event) {
	event->onConclusion.disconnect(boost::bind(&EventController::handleEventConcluded, this, event));
	events_.erase(std::remove(events_.begin(), events_.end(), event), events_.end());
	onEventQueueLengthChange(boost::numeric_cast<int>(events_.size()));
}

void EventController::disconnectAll() {
	onEventQueueLengthChange.disconnect_all_slots();
	onEventQueueEventAdded.disconnect_all_slots();
}

void EventController::clear() {
	events_.clear();
	onEventQueueLengthChange(0);
}

}
