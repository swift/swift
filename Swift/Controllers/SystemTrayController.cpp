#include "Swift/Controllers/SystemTrayController.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/SystemTray.h"

namespace Swift {

SystemTrayController::SystemTrayController(EventController* eventController, SystemTray* systemTray) {
	eventController_ = eventController;
	systemTray_ = systemTray;
	eventController_->onEventQueueLengthChange.connect(boost::bind(&SystemTrayController::handleEventQueueLengthChange, this, _1));
}

void SystemTrayController::handleEventQueueLengthChange(int length) {
	systemTray_->setUnreadMessages(length > 0 ? true : false);
}


}
