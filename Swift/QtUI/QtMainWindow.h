/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtMainWindow_H
#define SWIFT_QtMainWindow_H

#include <QWidget>
#include <QMenu>
#include "Swift/Controllers/UIInterfaces/MainWindow.h"
#include "Swift/QtUI/QtRosterHeader.h"
#include "Swift/QtUI/EventViewer/QtEventWindow.h"
#include "Swift/QtUI/ChatList/QtChatListWindow.h"
#include "Swift/QtUI/ContextMenus/QtRosterContextMenu.h"

#include <vector>

class QComboBox;
class QLineEdit;
class QPushButton;
class QToolBar;
class QAction;

class QTabWidget;

namespace Swift {
	class QtTreeWidget;
	class QtTreeWidgetFactory;
	class TreeWidget;
	class UIEventStream;
	class QtTabWidget;
	class QtSettingsProvider;

	class QtMainWindow : public QWidget, public MainWindow {
		Q_OBJECT
		public:
			QtMainWindow(QtSettingsProvider*, UIEventStream* eventStream);
			~QtMainWindow();
			std::vector<QMenu*> getMenus() {return menus_;}
			void setMyNick(const String& name);
			void setMyJID(const JID& jid);
			void setMyAvatarPath(const String& path);
			void setMyStatusText(const String& status);
			void setMyStatusType(StatusShow::Type type);
			void setConnecting();
			QtEventWindow* getEventWindow();
			QtChatListWindow* getChatListWindow();
			void setRosterModel(Roster* roster);
		private slots:
			void handleStatusChanged(StatusShow::Type showType, const QString &statusMessage);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleShowOfflineToggled(bool);
			void handleJoinMUCAction();
			void handleSignOutAction();
			void handleAddUserActionTriggered(bool checked);
			void handleChatUserActionTriggered(bool checked);
			void handleEventCountUpdated(int count);

		private:
			std::vector<QMenu*> menus_;
			QtTreeWidget* treeWidget_;
			QtRosterHeader* meView_;
			QAction* addUserAction_;
			QAction* chatUserAction_;
			QAction* showOfflineAction_;
			QtTabWidget* tabs_;
			QWidget* contactsTabWidget_;
			QWidget* eventsTabWidget_;
			QtEventWindow* eventWindow_;
			QtChatListWindow* chatListWindow_;
			UIEventStream* uiEventStream_;
			QtRosterContextMenu* contextMenu_;
			bool lastOfflineState_;
	};
}

#endif

