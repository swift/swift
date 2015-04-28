/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QAbstractItemModel>
#include <QPersistentModelIndex>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>

#include <Swift/QtUI/ChatList/ChatListGroupItem.h>

namespace Swift {
	class ChatListMUCItem;

	class ChatListModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			ChatListModel();
			Qt::ItemFlags flags(const QModelIndex& index) const;
			void addMUCBookmark(const MUCBookmark& bookmark);
			void removeMUCBookmark(const MUCBookmark& bookmark);
			void addWhiteboardSession(const ChatListWindow::Chat& chat);
			void removeWhiteboardSession(const JID& jid);
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
			QModelIndex parent(const QModelIndex& index) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			ChatListItem* getItemForIndex(const QModelIndex& index) const;
			void clearBookmarks();
			void setRecents(const std::list<ChatListWindow::Chat>& recents);
			QMimeData* mimeData(const QModelIndexList& indexes) const;

			const ChatListMUCItem* getChatListMUCItem(const JID& roomJID) const;

		private:
			ChatListGroupItem* mucBookmarks_;
			ChatListGroupItem* recents_;
			ChatListGroupItem* whiteboards_;
			ChatListGroupItem* root_;

			QPersistentModelIndex mucBookmarksIndex_;
			QPersistentModelIndex recentsIndex_;
			QPersistentModelIndex whiteboardsIndex_;
	};

}
