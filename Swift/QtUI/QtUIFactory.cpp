/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtUIFactory.h>

#include <algorithm>

#include <QSplitter>

#include <Swiften/Base/Log.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

#include <Swift/Controllers/Settings/SettingsProviderHierachy.h>

#include <Swift/QtUI/MUCSearch/QtMUCSearchWindow.h>
#include <Swift/QtUI/QtAdHocCommandWindow.h>
#include <Swift/QtUI/QtBlockListEditorWindow.h>
#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtChatWindow.h>
#include <Swift/QtUI/QtChatWindowFactory.h>
#include <Swift/QtUI/QtContactEditWindow.h>
#include <Swift/QtUI/QtFdpFormSubmitWindow.h>
#include <Swift/QtUI/QtFileTransferListWidget.h>
#include <Swift/QtUI/QtHighlightNotificationConfigDialog.h>
#include <Swift/QtUI/QtHistoryWindow.h>
#include <Swift/QtUI/QtJoinMUCWindow.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtMainWindow.h>
#include <Swift/QtUI/QtProfileWindow.h>
#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swift/QtUI/QtSingleWindow.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtSystemTray.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/QtXMLConsoleWidget.h>
#include <Swift/QtUI/UserSearch/QtUserSearchWindow.h>
#include <Swift/QtUI/Whiteboard/QtWhiteboardWindow.h>

namespace Swift {

QtUIFactory::QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabs* tabs, QtSingleWindow* netbookSplitter, QtSystemTray* systemTray, TimerFactory* timerFactory, StatusCache* statusCache, AutoUpdater* autoUpdater, std::map<std::string, std::string>& emoticons, bool enableAdHocCommandOnJID) : settings_(settings), qtOnlySettings_(qtOnlySettings), tabs_(tabs), netbookSplitter_(netbookSplitter), systemTray_(systemTray), timerFactory_(timerFactory), lastMainWindow_(nullptr), loginWindow_(nullptr), statusCache_(statusCache), autoUpdater_(autoUpdater), emoticons_(emoticons), enableAdHocCommandOnJID_(enableAdHocCommandOnJID) {
    emoticonsExist_ = !emoticons_.empty();
    chatWindowFactory_ = new QtChatWindowFactory(netbookSplitter_, settings, qtOnlySettings, tabs_, ":/themes/Default/", emoticons_);
    chatFontSize_ = settings_->getSetting(QtUISettingConstants::CHATWINDOW_FONT_SIZE);
    historyFontSize_ = settings_->getSetting(QtUISettingConstants::HISTORYWINDOW_FONT_SIZE);
}

QtUIFactory::~QtUIFactory() {
    SWIFT_LOG(debug) << "Entering QtUIFactory destructor. chatWindows size:" << chatWindows_.size() << std::endl;
    for (auto chat : chatWindows_) {
        SWIFT_LOG_ASSERT(chat.isNull(), debug) << "QtUIFactory has active chat windows and has not been reset properly" << std::endl;
    }
    delete chatWindowFactory_;
}

XMLConsoleWidget* QtUIFactory::createXMLConsoleWidget() {
    QtXMLConsoleWidget* widget = new QtXMLConsoleWidget();
    tabs_->addTab(widget);
    showTabs();
    widget->show();
    return widget;
}

HistoryWindow* QtUIFactory::createHistoryWindow(UIEventStream* uiEventStream) {
    QtHistoryWindow* window = new QtHistoryWindow(settings_, uiEventStream);
    tabs_->addTab(window);
    showTabs();
    connect(window, SIGNAL(fontResized(int)), this, SLOT(handleHistoryWindowFontResized(int)));

    window->handleFontResized(historyFontSize_);
    window->show();
    return window;
}

void QtUIFactory::handleHistoryWindowFontResized(int size) {
    historyFontSize_ = size;
    settings_->storeSetting(QtUISettingConstants::HISTORYWINDOW_FONT_SIZE, size);
}

FileTransferListWidget* QtUIFactory::createFileTransferListWidget() {
    QtFileTransferListWidget* widget = new QtFileTransferListWidget();
    tabs_->addTab(widget);
    showTabs();
    widget->show();
    return widget;
}

MainWindow* QtUIFactory::createMainWindow(Chattables& chattables, UIEventStream* eventStream) {
    lastMainWindow_ = new QtMainWindow(chattables, settings_, eventStream, loginWindow_->getMenus(), statusCache_, emoticonsExist_, enableAdHocCommandOnJID_);
    tabs_->setViewMenu(lastMainWindow_->getMenus()[0]);
    return lastMainWindow_;
}

LoginWindow* QtUIFactory::createLoginWindow(UIEventStream* eventStream) {
    if (loginWindow_) {
        return loginWindow_;
    }
    loginWindow_ = new QtLoginWindow(eventStream, settings_, timerFactory_, autoUpdater_);
    netbookSplitter_->addAccount(loginWindow_, tabs_);
    connect(systemTray_, SIGNAL(clicked()), loginWindow_, SLOT(toggleBringToFront()));
    return loginWindow_;
}

EventWindow* QtUIFactory::createEventWindow() {
    return lastMainWindow_->getEventWindow();
}

ChatListWindow* QtUIFactory::createChatListWindow(UIEventStream*) {
    return lastMainWindow_->getChatListWindow();
}

MUCSearchWindow* QtUIFactory::createMUCSearchWindow() {
        return new QtMUCSearchWindow();
}

ChatWindow* QtUIFactory::createChatWindow(const JID& contact, UIEventStream* eventStream) {
    QtChatWindow* window = dynamic_cast<QtChatWindow*>(chatWindowFactory_->createChatWindow(contact, eventStream));

    // remove already closed and thereby deleted chat windows
    chatWindows_.erase(std::remove_if(chatWindows_.begin(), chatWindows_.end(),
        [](QPointer<QtChatWindow>& window) {
            return window.isNull();
        }), chatWindows_.end());

    chatWindows_.push_back(window);

    connect(window, SIGNAL(fontResized(int)), this, SLOT(handleChatWindowFontResized(int)));
    window->handleFontResized(chatFontSize_);
    return window;
}

void QtUIFactory::handleChatWindowFontResized(int size) {
    chatFontSize_ = size;
    settings_->storeSetting(QtUISettingConstants::CHATWINDOW_FONT_SIZE, size);

    // resize font in other chat windows
    for (auto&& existingWindow : chatWindows_) {
        if (!existingWindow.isNull()) {
            existingWindow->handleFontResized(size);
        }
    }
}

UserSearchWindow* QtUIFactory::createUserSearchWindow(UserSearchWindow::Type type, UIEventStream* eventStream, const std::set<std::string>& groups) {
    return new QtUserSearchWindow(eventStream, type, groups, qtOnlySettings_);
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

WhiteboardWindow* QtUIFactory::createWhiteboardWindow(std::shared_ptr<WhiteboardSession> whiteboardSession) {
    return new QtWhiteboardWindow(whiteboardSession);
}

HighlightEditorWindow* QtUIFactory::createHighlightEditorWindow() {
    return new QtHighlightNotificationConfigDialog(qtOnlySettings_);
}

BlockListEditorWidget *QtUIFactory::createBlockListEditorWidget() {
    return new QtBlockListEditorWindow();
}

AdHocCommandWindow* QtUIFactory::createAdHocCommandWindow(std::shared_ptr<OutgoingAdHocCommandSession> command) {
    return new QtAdHocCommandWindow(command);
}

std::unique_ptr<FdpFormSubmitWindow> QtUIFactory::createFdpFormSubmitWindow() {
    return std::make_unique<QtFdpFormSubmitWindow>();
}

void QtUIFactory::showTabs() {
    if (!tabs_->isVisible()) {
        tabs_->show();
    }
}

}
