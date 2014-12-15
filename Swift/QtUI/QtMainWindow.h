/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <QWidget>
#include <QMenu>
#include <QList>

#include <Swift/Controllers/UIInterfaces/MainWindow.h>

#include <Swift/QtUI/QtRosterHeader.h>
#include <Swift/QtUI/EventViewer/QtEventWindow.h>
#include <Swift/QtUI/ChatList/QtChatListWindow.h>
#include <Swift/QtUI/QtLoginWindow.h>

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
	class StatusCache;

	class QtMainWindow : public QWidget, public MainWindow {
		Q_OBJECT
		public:
			QtMainWindow(SettingsProvider*, UIEventStream* eventStream, QtLoginWindow::QtMenus loginMenus, StatusCache* statusCache, bool emoticonsExist, bool enableAdHocCommandOnJID);
			virtual ~QtMainWindow();
			std::vector<QMenu*> getMenus() {return menus_;}
			void setMyNick(const std::string& name);
			void setMyJID(const JID& jid);
			void setMyAvatarPath(const std::string& path);
			void setMyStatusText(const std::string& status);
			void setMyStatusType(StatusShow::Type type);
			void setMyContactRosterItem(boost::shared_ptr<ContactRosterItem> contact);
			void setConnecting();
			void setStreamEncryptionStatus(bool tlsInPlaceAndValid);
			void openCertificateDialog(const std::vector<Certificate::ref>& chain);
			static void openCertificateDialog(const std::vector<Certificate::ref>& chain, QWidget* parent);
			QtEventWindow* getEventWindow();
			QtChatListWindow* getChatListWindow();
			void setRosterModel(Roster* roster);
			void setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands);
			void setBlockingCommandAvailable(bool isAvailable);
		private slots:
			void handleStatusChanged(StatusShow::Type showType, const QString &statusMessage);
			void handleSettingChanged(const std::string& settingPath);
			void handleCompactRosterToggled(bool);
			void handleShowOfflineToggled(bool);
			void handleShowEmoticonsToggled(bool);
			void handleJoinMUCAction();
			void handleViewLogsAction();
			void handleSignOutAction();
			void handleEditProfileAction();
			void handleAddUserActionTriggered(bool checked);
			void handleChatUserActionTriggered(bool checked);
			void handleOtherAdHocActionTriggered();
			void handleAdHocActionTriggered(bool checked);
			void handleEventCountUpdated(int count);
			void handleChatCountUpdated(int count);
			void handleEditProfileRequest();
			void handleTabChanged(int index);
			void handleToggleRequestDeliveryReceipts(bool enabled);
			void handleShowCertificateInfo();
			void handleEditBlockingList();
			void handleSomethingSelectedChanged(bool itemSelected);

		private:
			SettingsProvider* settings_;
			QtLoginWindow::QtMenus loginMenus_;
			std::vector<QMenu*> menus_;
			QtRosterWidget* treeWidget_;
			QtRosterHeader* meView_;
			QAction* addUserAction_;
			QAction* editUserAction_;
			QAction* chatUserAction_;
			QAction* otherAdHocAction_;
			QAction* showOfflineAction_;
			QAction* compactRosterAction_;
			QAction* showEmoticonsAction_;
			QAction* openBlockingListEditor_;
			QAction* toggleRequestDeliveryReceipts_;
			QMenu* serverAdHocMenu_;
			QtTabWidget* tabs_;
			QComboBox* tabBarCombo_;
			QWidget* contactsTabWidget_;
			QWidget* eventsTabWidget_;
			QtEventWindow* eventWindow_;
			QtChatListWindow* chatListWindow_;
			UIEventStream* uiEventStream_;
			std::vector<DiscoItems::Item> serverAdHocCommands_;
			QList<QAction*> serverAdHocCommandActions_;
			QList<QAction*> onlineOnlyActions_;
	};
}
