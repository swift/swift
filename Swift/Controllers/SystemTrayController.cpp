/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/SystemTrayController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/SystemTray.h"

namespace Swift {

SystemTrayController::SystemTrayController(EventController* eventController, SystemTray* systemTray) {
	systemTray_ = systemTray;
	eventController_ = eventController;
	eventController_->onEventQueueLengthChange.connect(boost::bind(&SystemTrayController::handleEventQueueLengthChange, this, _1));
}

void SystemTrayController::handleEventQueueLengthChange(int /*length*/) {
	EventList events = eventController_->getEvents();
	bool found = false;
	for (EventList::iterator it = events.begin(); it != events.end(); ++it) {
		if (boost::dynamic_pointer_cast<MessageEvent>(*it)) {
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
