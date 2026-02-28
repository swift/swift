/*
 * Copyright (c) 2010-2018 Isode Limited.
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
    class Chattables;
    class FdpFormSubmitWindow;
    class QtChatTabs;
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
            QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabs* tabs, QtSingleWindow* netbookSplitter, QtSystemTray* systemTray, TimerFactory* timerFactory, StatusCache* statusCache, AutoUpdater* autoUpdater, std::map<std::string, std::string>& emoticons, bool enableAdHocCommandOnJID);
            ~QtUIFactory();
            virtual XMLConsoleWidget* createXMLConsoleWidget();
            virtual HistoryWindow* createHistoryWindow(UIEventStream*);
            virtual MainWindow* createMainWindow(Chattables& chattables, UIEventStream* eventStream);
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
            virtual std::unique_ptr<FdpFormSubmitWindow> createFdpFormSubmitWindow();

        private slots:
            void handleChatWindowFontResized(int);
            void handleHistoryWindowFontResized(int);

        private:
            void showTabs();

        private:
            SettingsProviderHierachy* settings_;
            QtSettingsProvider* qtOnlySettings_;
            QtChatTabs* tabs_;
            QtSingleWindow* netbookSplitter_;
            QtSystemTray* systemTray_;
            QtChatWindowFactory* chatWindowFactory_;
            TimerFactory* timerFactory_;
            QtMainWindow* lastMainWindow_;
            QtLoginWindow* loginWindow_;
            StatusCache* statusCache_;
            AutoUpdater* autoUpdater_;
            std::vector<QPointer<QtChatWindow> > chatWindows_;
            int chatFontSize_;
            int historyFontSize_;
            bool emoticonsExist_;
            std::map<std::string, std::string>& emoticons_;
            bool enableAdHocCommandOnJID_;
    };
}
