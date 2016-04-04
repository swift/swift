/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/SystemTrayController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/SystemTray.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

namespace Swift {

SystemTrayController::SystemTrayController(EventController* eventController, SystemTray* systemTray) {
    systemTray_ = systemTray;
    eventController_ = eventController;
    eventController_->onEventQueueLengthChange.connect(boost::bind(&SystemTrayController::handleEventQueueLengthChange, this, _1));
}

void SystemTrayController::handleEventQueueLengthChange(int /*length*/) {
    EventList events = eventController_->getEvents();
    bool found = false;
    for (auto& event : events) {
        if (std::dynamic_pointer_cast<MessageEvent>(event)) {
            found = true;
            break;
        }
    }
    systemTray_->setUnreadMessages(found);
}

void SystemTrayController::setMyStatusType(StatusShow::Type type) {
    systemTray_->setStatusType(type);
}

void SystemTrayController::setConnecting() {
    systemTray_->setConnecting();
}

}
