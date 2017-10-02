/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/XMPPEvents/EventController.h>

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swift/Controllers/XMPPEvents/ErrorEvent.h>
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

namespace Swift {

EventController::EventController() {
}

EventController::~EventController() {
    for (auto&& event : events_) {
        event->onConclusion.disconnect(boost::bind(&EventController::handleEventConcluded, this, event));
    }
}

void EventController::handleIncomingEvent(std::shared_ptr<StanzaEvent> sourceEvent) {
    std::shared_ptr<MessageEvent> messageEvent = std::dynamic_pointer_cast<MessageEvent>(sourceEvent);
    std::shared_ptr<SubscriptionRequestEvent> subscriptionEvent = std::dynamic_pointer_cast<SubscriptionRequestEvent>(sourceEvent);
    std::shared_ptr<ErrorEvent> errorEvent = std::dynamic_pointer_cast<ErrorEvent>(sourceEvent);
    std::shared_ptr<MUCInviteEvent> mucInviteEvent = std::dynamic_pointer_cast<MUCInviteEvent>(sourceEvent);
    std::shared_ptr<IncomingFileTransferEvent> incomingFileTransferEvent = std::dynamic_pointer_cast<IncomingFileTransferEvent>(sourceEvent);

    /* If it's a duplicate subscription request, remove the previous request first */
    if (subscriptionEvent) {
        EventList existingEvents(events_);
        for (auto&& existingEvent : existingEvents) {
            std::shared_ptr<SubscriptionRequestEvent> existingSubscriptionEvent = std::dynamic_pointer_cast<SubscriptionRequestEvent>(existingEvent);
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

void EventController::handleEventConcluded(std::shared_ptr<StanzaEvent> event) {
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
