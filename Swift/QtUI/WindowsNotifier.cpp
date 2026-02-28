/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/WindowsNotifier.h>

#include <cassert>
#include <iostream>

#include <boost/bind.hpp>

#include <QSystemTrayIcon>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtWin32NotifierWindow.h>

namespace Swift {

WindowsNotifier::WindowsNotifier(const std::string& name, const boost::filesystem::path& icon, QSystemTrayIcon* tray) : tray(tray) {
    notifierWindow = new QtWin32NotifierWindow();
    connect(tray, SIGNAL(messageClicked()), SLOT(handleMessageClicked()));
}

WindowsNotifier::~WindowsNotifier() {
    delete notifierWindow;
}

void WindowsNotifier::showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback) {
    std::vector<Notifier::Type> defaultTypes = getDefaultTypes();
    if (std::find(defaultTypes.begin(), defaultTypes.end(), type) == defaultTypes.end()) {
        return;
    }
    lastCallback = callback;
    int timeout = (type == IncomingMessage || type == SystemMessage) ? DEFAULT_MESSAGE_NOTIFICATION_TIMEOUT_SECONDS : DEFAULT_STATUS_NOTIFICATION_TIMEOUT_SECONDS;
    tray->showMessage(P2QSTRING(subject), P2QSTRING(description), type == SystemMessage ? QSystemTrayIcon::Information : QSystemTrayIcon::NoIcon, timeout * 1000);
}

void WindowsNotifier::handleMessageClicked() {
    if (lastCallback) {
        lastCallback();
    }
}

void WindowsNotifier::purgeCallbacks() {
    lastCallback = boost::function<void()>();
}

}
