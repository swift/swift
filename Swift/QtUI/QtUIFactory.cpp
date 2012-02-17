/*
 * Copyright (c) 2010-2012 Remko Tronçon
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
#include "QtAdHocCommandWindow.h"
#include "QtFileTransferListWidget.h"
#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/QtUI/QtUISettingConstants.h>

namespace Swift {

QtUIFactory::QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabs* tabs, QSplitter* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, bool startMinimized) : settings(settings), qtOnlySettings(qtOnlySettings), tabs(tabs), netbookSplitter(netbookSplitter), systemTray(systemTray), chatWindowFactory(chatWindowFactory), lastMainWindow(NULL), loginWindow(NULL), startMinimized(startMinimized) {
	chatFontSize = settings->getSetting(QtUISettingConstants::CHATWINDOW_FONT_SIZE);
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

FileTransferListWidget* QtUIFactory::createFileTransferListWidget() {
	QtFileTransferListWidget* widget = new QtFileTransferListWidget();
	tabs->addTab(widget);
	if (!tabs->isVisible()) {
		tabs->show();
	}
	widget->show();
	return widget;
}

MainWindow* QtUIFactory::createMainWindow(UIEventStream* eventStream) {
	lastMainWindow  = new QtMainWindow(settings, eventStream, loginWindow->getMenus());
	return lastMainWindow;
}

LoginWindow* QtUIFactory::createLoginWindow(UIEventStream* eventStream) {
	loginWindow = new QtLoginWindow(eventStream, settings);
	if (netbookSplitter) {
		netbookSplitter->insertWidget(0, loginWindow);
	}
	connect(systemTray, SIGNAL(clicked()), loginWindow, SLOT(bringToFront()));

#ifndef SWIFT_MOBILE
	QVariant loginWindowGeometryVariant = qtOnlySettings->getQSettings()->value("loginWindowGeometry");
	if (loginWindowGeometryVariant.isValid()) {
		loginWindow->restoreGeometry(loginWindowGeometryVariant.toByteArray());
	}
	connect(loginWindow, SIGNAL(geometryChanged()), this, SLOT(handleLoginWindowGeometryChanged()));
	if (startMinimized) loginWindow->hide();
#endif
	return loginWindow;
}

void QtUIFactory::handleLoginWindowGeometryChanged() {
	qtOnlySettings->getQSettings()->setValue("loginWindowGeometry", loginWindow->saveGeometry());
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
	QtChatWindow* window = dynamic_cast<QtChatWindow*>(chatWindowFactory->createChatWindow(contact, eventStream));
	chatWindows.push_back(window);
	std::vector<QPointer<QtChatWindow> > deletions;
	foreach (QPointer<QtChatWindow> existingWindow, chatWindows) {
		if (existingWindow.isNull()) {
			deletions.push_back(existingWindow);
		} else {
			connect(window, SIGNAL(fontResized(int)), existingWindow, SLOT(handleFontResized(int)));
			connect(existingWindow, SIGNAL(fontResized(int)), window, SLOT(handleFontResized(int)));
		}
	}
	foreach (QPointer<QtChatWindow> deletedWindow, deletions) {
		chatWindows.erase(std::remove(chatWindows.begin(), chatWindows.end(), deletedWindow), chatWindows.end());
	}
	connect(window, SIGNAL(fontResized(int)), this, SLOT(handleChatWindowFontResized(int)));
	window->handleFontResized(chatFontSize);
	return window;
}

void QtUIFactory::handleChatWindowFontResized(int size) {
	chatFontSize = size;
	settings->storeSetting(QtUISettingConstants::CHATWINDOW_FONT_SIZE, size);
}

UserSearchWindow* QtUIFactory::createUserSearchWindow(UserSearchWindow::Type type, UIEventStream* eventStream, const std::set<std::string>& groups) {
	return new QtUserSearchWindow(eventStream, type, groups);
};

JoinMUCWindow* QtUIFactory::createJoinMUCWindow(UIEventStream* uiEventStream) {
	return new QtJoinMUCWindow(uiEventStream);
}

ProfileWindow* QtUIFactory::createProfileWindow() {
	return new QtProfileWindow();
}

ContactEditWindow* QtUIFactory::createContactEditWindow() {
	return new QtContactEditWindow();
}

void QtUIFactory::createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) {
	new QtAdHocCommandWindow(command);
}

}
