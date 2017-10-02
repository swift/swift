/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/AdHocController.h>

#include <boost/bind.hpp>

#include <Swift/Controllers/UIInterfaces/AdHocCommandWindowFactory.h>

namespace Swift {

AdHocController::AdHocController(AdHocCommandWindowFactory* factory, std::shared_ptr<OutgoingAdHocCommandSession> command) {
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
