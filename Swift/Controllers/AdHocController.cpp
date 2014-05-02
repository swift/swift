/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/bind.hpp>
#include <Swift/Controllers/AdHocController.h>
#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>

namespace Swift {

AdHocController::AdHocController(AdHocCommandWindowFactory* factory, boost::shared_ptr<OutgoingAdHocCommandSession> command) {
	window_ = factory->createAdHocCommandWindow(command);
	window_->onClosing.connect(boost::bind(&AdHocController::handleWindowClosed, this));
}

AdHocController::~AdHocController() {
	window_->onClosing.disconnect(boost::bind(&AdHocController::handleWindowClosed, this));
	delete window_;
}

void AdHocController::setOnline(bool online) {
	window_->setOnline(online);
}

void AdHocController::handleWindowClosed() {
	onDeleting();
}

}
