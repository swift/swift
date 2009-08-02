#include "Slimber/MenuletController.h"

#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Slimber/Menulet.h"

#include <iostream>

using namespace Swift;

MenuletController::MenuletController(Menulet* menulet) : 
		menulet(menulet), xmppStatus(Offline) {
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
	menulet->addExitItem();
}
