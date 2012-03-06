/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QMenu>
#include <QList>
#include "Swift/Controllers/UIInterfaces/MainWindow.h"
#include "Swift/QtUI/QtRosterHeader.h"
#include "Swift/QtUI/EventViewer/QtEventWindow.h"
#include "Swift/QtUI/ChatList/QtChatListWindow.h"
#include "Swift/QtUI/QtLoginWindow.h"

#include <vector>

class QComboBox;
class QLineEdit;
class QPushButton;
class QToolBar;
class QAction;
class QMenu;
class QTabWidget;

namespace Swift {
	class QtRosterWidget;
	class TreeWidget;
	class UIEventStream;
	class QtTabWidget;
	class SettingsProvider;
	class QtUIPreferences;

	class QtMainWindow : public QWidget, public MainWindow {
		Q_OBJECT
		public:
			QtMainWindow(SettingsProvider*, UIEventStream* eventStream, QtLoginWindow::QtMenus loginMenus);
			virtual ~QtMainWindow();
			std::vector<QMenu*> getMenus() {return menus_;}
			void setMyNick(const std::string& name);
			void setMyJID(const JID& jid);
			void setMyAvatarPath(const std::string& path);
			void setMyStatusText(const std::string& status);
			void setMyStatusType(StatusShow::Type type);
			void setConnecting();
			QtEventWindow* getEventWindow();
			QtChatListWindow* getChatListWindow();
			void setRosterModel(Roster* roster);
			void setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands);
		private slots:
			void handleStatusChanged(StatusShow::Type showType, const QString &statusMessage);
			void handleSettingChanged(const std::string& settingPath);
			void handleShowOfflineToggled(bool);
			void handleJoinMUCAction();
			void handleSignOutAction();
			void handleEditProfileAction();
			void handleAddUserActionTriggered(bool checked);
			void handleChatUserActionTriggered(bool checked);
			void handleAdHocActionTriggered(bool checked);
			void handleEventCountUpdated(int count);
			void handleChatCountUpdated(int count);
			void handleEditProfileRequest();
			void handleTabChanged(int index);
			void handleToggleRequestDeliveryReceipts(bool enabled);

		private:
			SettingsProvider* settings_;
			QtLoginWindow::QtMenus loginMenus_;
			std::vector<QMenu*> menus_;
			QtRosterWidget* treeWidget_;
			QtRosterHeader* meView_;
			QAction* addUserAction_;
			QAction* editUserAction_;
			QAction* chatUserAction_;
			QAction* showOfflineAction_;
			QAction* toggleRequestDeliveryReceipts_;
			QMenu* serverAdHocMenu_;
			QtTabWidget* tabs_;
			QWidget* contactsTabWidget_;
			QWidget* eventsTabWidget_;
			QtEventWindow* eventWindow_;
			QtChatListWindow* chatListWindow_;
			UIEventStream* uiEventStream_;
			std::vector<DiscoItems::Item> serverAdHocCommands_;
			QList<QAction*> serverAdHocCommandActions_;
	};
}
