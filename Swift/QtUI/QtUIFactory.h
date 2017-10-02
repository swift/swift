/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <QObject>
#include <QPointer>

#include <Swift/Controllers/UIInterfaces/UIFactory.h>

class QSplitter;

namespace Swift {
    class AutoUpdater;
    class QtChatTabs;
    class QtChatTabsBase;
    class QtChatTheme;
    class QtChatWindow;
    class QtChatWindowFactory;
    class QtLoginWindow;
    class QtMainWindow;
    class QtSettingsProvider;
    class QtSingleWindow;
    class QtSystemTray;
    class SettingsProviderHierachy;
    class StatusCache;
    class TimerFactory;
    class WhiteboardSession;

    class QtUIFactory : public QObject, public UIFactory {
            Q_OBJECT
        public:
            QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabsBase* tabs, QtSingleWindow* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, TimerFactory* timerFactory, StatusCache* statusCache, AutoUpdater* autoUpdater, bool startMinimized, bool emoticonsExist, bool enableAdHocCommandOnJID);

            virtual XMLConsoleWidget* createXMLConsoleWidget();
            virtual HistoryWindow* createHistoryWindow(UIEventStream*);
            virtual MainWindow* createMainWindow(UIEventStream* eventStream);
            virtual LoginWindow* createLoginWindow(UIEventStream* eventStream);
            virtual EventWindow* createEventWindow();
            virtual ChatListWindow* createChatListWindow(UIEventStream*);
            virtual MUCSearchWindow* createMUCSearchWindow();
            virtual ChatWindow* createChatWindow(const JID &contact, UIEventStream* eventStream);
            virtual UserSearchWindow* createUserSearchWindow(UserSearchWindow::Type type, UIEventStream* eventStream, const std::set<std::string>& groups);
            virtual JoinMUCWindow* createJoinMUCWindow(UIEventStream* uiEventStream);
            virtual ProfileWindow* createProfileWindow();
            virtual ContactEditWindow* createContactEditWindow();
            virtual FileTransferListWidget* createFileTransferListWidget();
            virtual WhiteboardWindow* createWhiteboardWindow(std::shared_ptr<WhiteboardSession> whiteboardSession);
            virtual HighlightEditorWindow* createHighlightEditorWindow();
            virtual BlockListEditorWidget* createBlockListEditorWidget();
            virtual AdHocCommandWindow* createAdHocCommandWindow(std::shared_ptr<OutgoingAdHocCommandSession> command);

        private slots:
            void handleLoginWindowGeometryChanged();
            void handleChatWindowFontResized(int);
            void handleHistoryWindowFontResized(int);

        private:
            void showTabs();

        private:
            SettingsProviderHierachy* settings;
            QtSettingsProvider* qtOnlySettings;
            QtChatTabsBase* tabsBase;
            QtChatTabs* tabs;
            QtSingleWindow* netbookSplitter;
            QtSystemTray* systemTray;
            QtChatWindowFactory* chatWindowFactory;
            TimerFactory* timerFactory_;
            QtMainWindow* lastMainWindow;
            QtLoginWindow* loginWindow;
            StatusCache* statusCache;
            AutoUpdater* autoUpdater;
            std::vector<QPointer<QtChatWindow> > chatWindows;
            bool startMinimized;
            int chatFontSize;
            int historyFontSize_;
            bool emoticonsExist_;
            bool enableAdHocCommandOnJID_;
    };
}
