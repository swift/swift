/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <SwifTools/Notifier/SnarlNotifier.h>

#include <cassert>
#include <iostream>
#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <SwifTools/Notifier/Win32NotifierWindow.h>

#define SWIFT_SNARLNOTIFIER_MESSAGE_ID 0x4567 // Sounds sick to pick a number, but this is windows

namespace Swift {

SnarlNotifier::SnarlNotifier(const std::string& name, Win32NotifierWindow* window, const boost::filesystem::path& icon) : window(window), available(false) {
	window->onMessageReceived.connect(boost::bind(&SnarlNotifier::handleMessageReceived, this, _1));
	available = snarl.RegisterApp(name.c_str(), name.c_str(), icon.string().c_str(), window->getID(), SWIFT_SNARLNOTIFIER_MESSAGE_ID);
	foreach(Notifier::Type type, getAllTypes()) {
		snarl.AddClass(typeToString(type).c_str(), typeToString(type).c_str());
	}
}

SnarlNotifier::~SnarlNotifier() {
	snarl.UnregisterApp();
	window->onMessageReceived.disconnect(boost::bind(&SnarlNotifier::handleMessageReceived, this, _1));
	if (!notifications.empty()) {
		std::cerr << "Warning: " << notifications.size() << " Snarl notifications pending" << std::endl;
	}
}

bool SnarlNotifier::isAvailable() const {
	return available;
}


void SnarlNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback) {
	int timeout = (type == IncomingMessage || type == SystemMessage) ? DEFAULT_MESSAGE_NOTIFICATION_TIMEOUT_SECONDS : DEFAULT_STATUS_NOTIFICATION_TIMEOUT_SECONDS;
	int notificationID = snarl.EZNotify(
			typeToString(type).c_str(),
			subject.c_str(),
			description.c_str(),
			timeout,
			picture.string().c_str());
	if (notificationID > 0) {
		notifications.insert(std::make_pair(notificationID, callback));
	}
}

void SnarlNotifier::handleMessageReceived(MSG* message) {
	if (message->message == SWIFT_SNARLNOTIFIER_MESSAGE_ID) {
		int action = message->wParam;
		if (action == Snarl::V41::SnarlEnums::NotificationTimedOut || action == Snarl::V41::SnarlEnums::NotificationAck || action == Snarl::V41::SnarlEnums::NotificationClosed) {
			int notificationID = message->lParam;
			NotificationsMap::iterator i = notifications.find(notificationID);
			if (i != notifications.end()) {
				if (action == Snarl::V41::SnarlEnums::NotificationAck && !i->second.empty()) {
					i->second();
				}
				notifications.erase(i);
			}
			else {
				std::cerr << "Warning: Orphaned Snarl notification received";
			}
		}
	}
}

}
