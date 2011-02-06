/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QTreeView>

#include "Swift/Controllers/UIInterfaces/ChatListWindow.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/QtUI/ChatList/ChatListModel.h"
#include "Swift/QtUI/ChatList/ChatListDelegate.h"

namespace Swift {

	class QtChatListWindow : public QTreeView, public ChatListWindow {
		Q_OBJECT
		public:
			QtChatListWindow(UIEventStream *uiEventStream, QWidget* parent = NULL);
			virtual ~QtChatListWindow();
			void addMUCBookmark(const MUCBookmark& bookmark);
			void removeMUCBookmark(const MUCBookmark& bookmark);
			void setBookmarksEnabled(bool enabled);
			void clear();
		private slots:
			void handleItemActivated(const QModelIndex&);
			void handleAddBookmark();
			void handleEditBookmark();
			void handleRemoveBookmark();
			void handleClicked(const QModelIndex& index);

		protected:
			void contextMenuEvent(QContextMenuEvent* event);

		private:
			void setupContextMenus();
			bool bookmarksEnabled_;
			UIEventStream* eventStream_;
			ChatListModel* model_;
			ChatListDelegate* delegate_;
			QMenu* mucMenu_;
			QMenu* emptyMenu_;
			ChatListItem* contextMenuItem_;
	};

}
