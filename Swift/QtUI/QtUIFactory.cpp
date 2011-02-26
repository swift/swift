/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtUIFactory.h"

#include <QSplitter>

#include "QtXMLConsoleWidget.h"
#include "QtChatTabs.h"
#include "QtMainWindow.h"
#include "QtLoginWindow.h"
#include "QtSystemTray.h"
#include "QtSettingsProvider.h"
#include "QtMainWindow.h"
#include "QtChatWindow.h"
#include "QtJoinMUCWindow.h"
#include "QtChatWindowFactory.h"
#include "QtSwiftUtil.h"
#include "MUCSearch/QtMUCSearchWindow.h"
#include "UserSearch/QtUserSearchWindow.h"
#include "QtProfileWindow.h"
#include "QtContactEditWindow.h"

namespace Swift {

QtUIFactory::QtUIFactory(QtSettingsProvider* settings, QtChatTabs* tabs, QSplitter* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, bool startMinimized) : settings(settings), tabs(tabs), netbookSplitter(netbookSplitter), systemTray(systemTray), chatWindowFactory(chatWindowFactory), lastMainWindow(NULL), loginWindow(NULL), startMinimized(startMinimized)  {
}

XMLConsoleWidget* QtUIFactory::createXMLConsoleWidget() {
	QtXMLConsoleWidget* widget = new QtXMLConsoleWidget();
	tabs->addTab(widget);
	if (!tabs->isVisible()) {
		tabs->show();
	}
	widget->show();
	return widget;
}


MainWindow* QtUIFactory::createMainWindow(UIEventStream* eventStream) {
	lastMainWindow  = new QtMainWindow(settings, eventStream);
	return lastMainWindow;
}

LoginWindow* QtUIFactory::createLoginWindow(UIEventStream* eventStream) {
	loginWindow = new QtLoginWindow(eventStream);
	if (netbookSplitter) {
		netbookSplitter->insertWidget(0, loginWindow);
	}
	connect(systemTray, SIGNAL(clicked()), loginWindow, SLOT(bringToFront()));

#ifndef SWIFT_MOBILE
	QVariant loginWindowGeometryVariant = settings->getQSettings()->value("loginWindowGeometry");
	if (loginWindowGeometryVariant.isValid()) {
		loginWindow->restoreGeometry(loginWindowGeometryVariant.toByteArray());
	}
	connect(loginWindow, SIGNAL(geometryChanged()), this, SLOT(handleLoginWindowGeometryChanged()));
	if (startMinimized) loginWindow->hide();
#endif
	return loginWindow;
}

void QtUIFactory::handleLoginWindowGeometryChanged() {
	settings->getQSettings()->setValue("loginWindowGeometry", loginWindow->saveGeometry());
}

EventWindow* QtUIFactory::createEventWindow() {
	return lastMainWindow->getEventWindow();
}

ChatListWindow* QtUIFactory::createChatListWindow(UIEventStream*) {
	return lastMainWindow->getChatListWindow();
}

MUCSearchWindow* QtUIFactory::createMUCSearchWindow() {
		return new QtMUCSearchWindow();
}

ChatWindow* QtUIFactory::createChatWindow(const JID& contact, UIEventStream* eventStream) {
	return chatWindowFactory->createChatWindow(contact, eventStream);
}

UserSearchWindow* QtUIFactory::createUserSearchWindow(UserSearchWindow::Type type, UIEventStream* eventStream, const std::set<std::string>& groups) {
	return new QtUserSearchWindow(eventStream, type, groups);
};

JoinMUCWindow* QtUIFactory::createJoinMUCWindow() {
	return new QtJoinMUCWindow();
}

ProfileWindow* QtUIFactory::createProfileWindow() {
	return new QtProfileWindow();
}

ContactEditWindow* QtUIFactory::createContactEditWindow() {
	return new QtContactEditWindow();
}


}
