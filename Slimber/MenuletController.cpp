/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Slimber/MenuletController.h"

#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Slimber/Menulet.h"

#include <iostream>

using namespace Swift;

MenuletController::MenuletController(Menulet* menulet) : 
		menulet(menulet), xmppStatus(Offline) {
	menulet->onRestartClicked.connect(boost::ref(onRestartRequested));
	update();
}

MenuletController::~MenuletController() {
}

void MenuletController::setXMPPStatus(const String& message, Status status) {
	xmppStatus = status;
	xmppStatusMessage = message;
	update();
}

void MenuletController::setUserNames(const std::vector<String>& users) {
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
		foreach(const String& user, linkLocalUsers) {
			menulet->addItem(String("  ") + user);
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
