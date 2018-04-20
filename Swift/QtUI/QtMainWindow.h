/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <QList>
#include <QMenu>
#include <QWidget>

#include <Swift/Controllers/Chat/Chattables.h>
#include <Swift/Controllers/UIInterfaces/MainWindow.h>

#include <Swift/QtUI/ChatList/QtChatListWindow.h>
#include <Swift/QtUI/EventViewer/QtEventWindow.h>
#include <Swift/QtUI/QtLoginWindow.h>
#include <Swift/QtUI/QtRosterHeader.h>

class QAction;
class QComboBox;
class QLineEdit;
class QMenu;
class QPushButton;
class QTabWidget;
class QToolBar;

namespace Swift {
    class QtChatOverview;
    class QtRosterWidget;
    class QtTabWidget;
    class QtUIPreferences;
    class SettingsProvider;
    class StatusCache;
    class TreeWidget;
    class UIEventStream;

    class QtMainWindow : public QWidget, public MainWindow {
        Q_OBJECT
        public:
            QtMainWindow(Chattables&, SettingsProvider*, UIEventStream* eventStream, QtLoginWindow::QtMenus loginMenus, StatusCache* statusCache, bool emoticonsExist, bool enableAdHocCommandOnJID);
            virtual ~QtMainWindow() override;
            std::vector<QMenu*> getMenus() {return menus_;}
            void setMyNick(const std::string& name) override;
            void setMyJID(const JID& jid) override;
            void setMyAvatarPath(const std::string& path) override;
            void setMyStatusText(const std::string& status) override;
            void setMyStatusType(StatusShow::Type type) override;
            void setMyContactRosterItem(std::shared_ptr<ContactRosterItem> contact) override;
            void setConnecting() override;
            void setStreamEncryptionStatus(bool tlsInPlaceAndValid) override;
            void openCertificateDialog(const std::vector<Certificate::ref>& chain) override;
            static void openCertificateDialog(const std::vector<Certificate::ref>& chain, QWidget* parent);
            QtEventWindow* getEventWindow();
            QtChatListWindow* getChatListWindow();
            void setRosterModel(Roster* roster) override;
            void setAvailableAdHocCommands(const std::vector<DiscoItems::Item>& commands) override;
            void setBlockingCommandAvailable(bool isAvailable) override;
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
            void handleSubmitFormActionTriggered();

        private:
            Chattables& chattables_;
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
            QtChatOverview* chatOverview_;
            QWidget* contactsTabWidget_;
            QWidget* eventsTabWidget_;
            QtEventWindow* eventWindow_;
            QtChatListWindow* chatListWindow_;
            UIEventStream* uiEventStream_;
            std::vector<DiscoItems::Item> serverAdHocCommands_;
            QList<QAction*> serverAdHocCommandActions_;
            QList<QAction*> onlineOnlyActions_;
            QAction* submitFormAction_;
    };
}
