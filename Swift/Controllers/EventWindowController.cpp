/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/EventWindowController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h>

namespace Swift {

EventWindowController::EventWindowController(EventController* eventController, EventWindowFactory* windowFactory) {
    eventController_ = eventController;
    windowFactory_ = windowFactory;
    window_ = windowFactory_->createEventWindow();
    eventAddedConnection_ = eventController_->onEventQueueEventAdded.connect(boost::bind(&EventWindowController::handleEventQueueEventAdded, this, _1));
}

EventWindowController::~EventWindowController() {
    if (window_->canDelete()) {
        delete window_;
    }
}

void EventWindowController::handleEventQueueEventAdded(std::shared_ptr<StanzaEvent> event) {
    if (event->getConcluded()) {
        handleEventConcluded(event);
    } else {
        std::shared_ptr<MessageEvent> message = std::dynamic_pointer_cast<MessageEvent>(event);
        if (!(message && message->isReadable())) {
            event->onConclusion.connect(boost::bind(&EventWindowController::handleEventConcluded, this, event));
            window_->addEvent(event, true);
        }
    }
}

void EventWindowController::handleEventConcluded(std::shared_ptr<StanzaEvent> event) {
    window_->removeEvent(event);
    bool includeAsCompleted = true;
    /* Because subscription requests get duplicated, don't add them back */
    if (std::dynamic_pointer_cast<SubscriptionRequestEvent>(event) || std::dynamic_pointer_cast<MessageEvent>(event)) {
        includeAsCompleted = false;
    }
    if (includeAsCompleted) {
        window_->addEvent(event, false);
    }
    event->onConclusion.disconnect(boost::bind(&EventWindowController::handleEventConcluded, this, event));
}

}
