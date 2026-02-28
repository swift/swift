/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Slimber/MenuletController.h>

#include <string>

#include <Slimber/Menulet.h>

MenuletController::MenuletController(Menulet* menulet) :
        menulet(menulet), xmppStatus(Offline) {
    menulet->onRestartClicked.connect(boost::ref(onRestartRequested));
    update();
}

MenuletController::~MenuletController() {
}

void MenuletController::setXMPPStatus(const std::string& message, Status status) {
    xmppStatus = status;
    xmppStatusMessage = message;
    update();
}

void MenuletController::setUserNames(const std::vector<std::string>& users) {
    linkLocalUsers = users;
    update();
}

void MenuletController::update() {
    menulet->clear();
    if (linkLocalUsers.empty()) {
        menulet->setIcon("UsersOffline");
        menulet->addItem("No online users");
    }
    else {
        menulet->setIcon("UsersOnline");
        menulet->addItem("Online users:");
        for (const auto& user : linkLocalUsers) {
            menulet->addItem(std::string("  ") + user);
        }
    }
    menulet->addSeparator();
    menulet->addItem(xmppStatusMessage, (xmppStatus == Online ? "Online" : "Offline"));
    menulet->addSeparator();
    menulet->addAboutItem();
    menulet->addSeparator();
    menulet->addRestartItem();
    menulet->addExitItem();
}
