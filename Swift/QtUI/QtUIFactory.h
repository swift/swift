/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>
#include <QPointer>

#include <Swift/Controllers/UIInterfaces/UIFactory.h>

class QSplitter;

namespace Swift {
	class QtSettingsProvider;
	class SettingsProviderHierachy;
	class QtChatTabsBase;
	class QtChatTabs;
	class QtSystemTray;
	class QtLoginWindow;
	class QtMainWindow;
	class QtChatTheme;
	class QtChatWindowFactory;
	class QtChatWindow;
	class TimerFactory;
	class historyWindow_;
	class WhiteboardSession;
	class StatusCache;
	class QtSingleWindow;

	class QtUIFactory : public QObject, public UIFactory {
			Q_OBJECT
		public:
			QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabsBase* tabs, QtSingleWindow* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, TimerFactory* timerFactory, StatusCache* statusCache, bool startMinimized, bool emoticonsExist, bool enableAdHocCommandOnJID);

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
			virtual WhiteboardWindow* createWhiteboardWindow(boost::shared_ptr<WhiteboardSession> whiteboardSession);
			virtual HighlightEditorWindow* createHighlightEditorWindow();
			virtual BlockListEditorWidget* createBlockListEditorWidget();
			virtual AdHocCommandWindow* createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command);

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
			std::vector<QPointer<QtChatWindow> > chatWindows;
			bool startMinimized;
			int chatFontSize;
			int historyFontSize_;
			bool emoticonsExist_;
			bool enableAdHocCommandOnJID_;
	};
}
