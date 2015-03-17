/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtUIFactory.h>

#include <QSplitter>

#include <Swift/QtUI/QtXMLConsoleWidget.h>
#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtChatTabsBase.h>
#include <Swift/QtUI/QtMainWindow.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtSystemTray.h>
#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtMainWindow.h>
#include <Swift/QtUI/QtChatWindow.h>
#include <Swift/QtUI/QtJoinMUCWindow.h>
#include <Swift/QtUI/QtChatWindowFactory.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/MUCSearch/QtMUCSearchWindow.h>
#include <Swift/QtUI/UserSearch/QtUserSearchWindow.h>
#include <Swift/QtUI/QtProfileWindow.h>
#include <Swift/QtUI/QtContactEditWindow.h>
#include <Swift/QtUI/QtAdHocCommandWindow.h>
#include <Swift/QtUI/QtFileTransferListWidget.h>
#include <Swift/QtUI/QtHighlightEditor.h>
#include <Swift/QtUI/Whiteboard/QtWhiteboardWindow.h>
#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/QtHistoryWindow.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>
#include <Swift/QtUI/QtSingleWindow.h>
#include <Swift/QtUI/QtBlockListEditorWindow.h>

namespace Swift {

QtUIFactory::QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabsBase* tabs, QtSingleWindow* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, TimerFactory* timerFactory, StatusCache* statusCache, bool startMinimized, bool emoticonsExist, bool enableAdHocCommandOnJID) : settings(settings), qtOnlySettings(qtOnlySettings), tabsBase(tabs), netbookSplitter(netbookSplitter), systemTray(systemTray), chatWindowFactory(chatWindowFactory), timerFactory_(timerFactory), lastMainWindow(NULL), loginWindow(NULL), statusCache(statusCache), startMinimized(startMinimized), emoticonsExist_(emoticonsExist), enableAdHocCommandOnJID_(enableAdHocCommandOnJID) {
	chatFontSize = settings->getSetting(QtUISettingConstants::CHATWINDOW_FONT_SIZE);
	historyFontSize_ = settings->getSetting(QtUISettingConstants::HISTORYWINDOW_FONT_SIZE);
	this->tabs = dynamic_cast<QtChatTabs*>(tabsBase);
}

XMLConsoleWidget* QtUIFactory::createXMLConsoleWidget() {
	QtXMLConsoleWidget* widget = new QtXMLConsoleWidget();
	tabsBase->addTab(widget);
	showTabs();
	widget->show();
	return widget;
}

HistoryWindow* QtUIFactory::createHistoryWindow(UIEventStream* uiEventStream) {
	QtHistoryWindow* window = new QtHistoryWindow(settings, uiEventStream);
	tabsBase->addTab(window);
	showTabs();
	connect(window, SIGNAL(fontResized(int)), this, SLOT(handleHistoryWindowFontResized(int)));

	window->handleFontResized(historyFontSize_);
	window->show();
	return window;
}

void QtUIFactory::handleHistoryWindowFontResized(int size) {
	historyFontSize_ = size;
	settings->storeSetting(QtUISettingConstants::HISTORYWINDOW_FONT_SIZE, size);
}

FileTransferListWidget* QtUIFactory::createFileTransferListWidget() {
	QtFileTransferListWidget* widget = new QtFileTransferListWidget();
	tabsBase->addTab(widget);
	showTabs();
	widget->show();
	return widget;
}

MainWindow* QtUIFactory::createMainWindow(UIEventStream* eventStream) {
	lastMainWindow  = new QtMainWindow(settings, eventStream, loginWindow->getMenus(), statusCache, emoticonsExist_, enableAdHocCommandOnJID_);
	if (tabs) {
		tabs->setViewMenu(lastMainWindow->getMenus()[0]);
	}
	return lastMainWindow;
}

LoginWindow* QtUIFactory::createLoginWindow(UIEventStream* eventStream) {
	loginWindow = new QtLoginWindow(eventStream, settings, timerFactory_);
	if (netbookSplitter) {
		netbookSplitter->insertAtFront(loginWindow);
	}
	connect(systemTray, SIGNAL(clicked()), loginWindow, SLOT(toggleBringToFront()));

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
	return new QtUserSearchWindow(eventStream, type, groups, qtOnlySettings);
}

JoinMUCWindow* QtUIFactory::createJoinMUCWindow(UIEventStream* uiEventStream) {
	return new QtJoinMUCWindow(uiEventStream);
}

ProfileWindow* QtUIFactory::createProfileWindow() {
	return new QtProfileWindow();
}

ContactEditWindow* QtUIFactory::createContactEditWindow() {
	return new QtContactEditWindow();
}

WhiteboardWindow* QtUIFactory::createWhiteboardWindow(boost::shared_ptr<WhiteboardSession> whiteboardSession) {
	return new QtWhiteboardWindow(whiteboardSession);
}

HighlightEditorWindow* QtUIFactory::createHighlightEditorWindow() {
	return new QtHighlightEditor(qtOnlySettings);
}

BlockListEditorWidget *QtUIFactory::createBlockListEditorWidget() {
	return new QtBlockListEditorWindow();
}

AdHocCommandWindow* QtUIFactory::createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command) {
	return new QtAdHocCommandWindow(command);
}

void QtUIFactory::showTabs() {
	if (tabs) {
		if (!tabs->isVisible()) {
			tabs->show();
		}
	}
}

}
