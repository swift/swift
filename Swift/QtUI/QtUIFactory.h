/*
 * Copyright (c) 2010-2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>
#include <QPointer>

#include <Swift/Controllers/UIInterfaces/UIFactory.h>

class QSplitter;

namespace Swift {
	class QtSettingsProvider;
	class SettingsProviderHierachy;
	class QtChatTabs;
	class QtSystemTray;
	class QtLoginWindow;
	class QtMainWindow;
	class QtChatTheme;
	class QtChatWindowFactory;
	class QtChatWindow;

	class QtUIFactory : public QObject, public UIFactory {
			Q_OBJECT
		public:
			QtUIFactory(SettingsProviderHierachy* settings, QtSettingsProvider* qtOnlySettings, QtChatTabs* tabs, QSplitter* netbookSplitter, QtSystemTray* systemTray, QtChatWindowFactory* chatWindowFactory, bool startMinimized);

			virtual XMLConsoleWidget* createXMLConsoleWidget();
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
			virtual void createAdHocCommandWindow(boost::shared_ptr<OutgoingAdHocCommandSession> command);

		private slots:
			void handleLoginWindowGeometryChanged();
			void handleChatWindowFontResized(int);

		private:
			SettingsProviderHierachy* settings;
			QtSettingsProvider* qtOnlySettings;
			QtChatTabs* tabs;
			QSplitter* netbookSplitter;
			QtSystemTray* systemTray;
			QtChatWindowFactory* chatWindowFactory;
			QtMainWindow* lastMainWindow;
			QtLoginWindow* loginWindow;
			std::vector<QPointer<QtChatWindow> > chatWindows;
			bool startMinimized;
			int chatFontSize;
	};
}
