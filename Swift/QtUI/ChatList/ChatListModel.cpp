/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/ChatList/ChatListModel.h>

#include <QMimeData>
#include <QUrl>

#include <Swift/QtUI/ChatList/ChatListMUCItem.h>
#include <Swift/QtUI/ChatList/ChatListRecentItem.h>
#include <Swift/QtUI/ChatList/ChatListWhiteboardItem.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

ChatListModel::ChatListModel() : whiteboards_(NULL) {
	root_ = new ChatListGroupItem("", NULL, false);
	mucBookmarks_ = new ChatListGroupItem(tr("Bookmarked Rooms"), root_);
	recents_ = new ChatListGroupItem(tr("Recent Chats"), root_, false);
#ifdef SWIFT_EXPERIMENTAL_WB
	whiteboards_ = new ChatListGroupItem(tr("Opened Whiteboards"), root_, false);
	root_->addItem(whiteboards_);
#endif

	root_->addItem(recents_);
	root_->addItem(mucBookmarks_);

	QModelIndex idx = index(0, 0, QModelIndex());
	while (idx.isValid()) {
		if (idx.internalPointer() == mucBookmarks_) {
			mucBookmarksIndex_ = idx;
		} else if (idx.internalPointer() == recents_) {
			recentsIndex_ = idx;
		} else if (idx.internalPointer() == whiteboards_) {
			whiteboardsIndex_ = idx;
		}
		idx = index(idx.row() + 1, 0, QModelIndex());
	}
}

Qt::ItemFlags ChatListModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	if (dynamic_cast<ChatListRecentItem*>(getItemForIndex(index))) {
		flags |= Qt::ItemIsDragEnabled;
	}
	return flags;
}

void ChatListModel::clearBookmarks() {
	beginRemoveRows(mucBookmarksIndex_, 0, mucBookmarks_->rowCount());
	mucBookmarks_->clear();
	endRemoveRows();
}

void ChatListModel::addMUCBookmark(const Swift::MUCBookmark& bookmark) {
	beginInsertRows(mucBookmarksIndex_, 0, mucBookmarks_->rowCount());
	mucBookmarks_->addItem(new ChatListMUCItem(bookmark, mucBookmarks_));
	endInsertRows();
}

void ChatListModel::removeMUCBookmark(const Swift::MUCBookmark& bookmark) {
	for (int i = 0; i < mucBookmarks_->rowCount(); i++) {
		ChatListMUCItem* item = dynamic_cast<ChatListMUCItem*>(mucBookmarks_->item(i));
		if (item->getBookmark() == bookmark) {
			beginRemoveRows(mucBookmarksIndex_, i, i+1);
			mucBookmarks_->remove(i);
			endRemoveRows();
			break;
		}
	}
}

void ChatListModel::addWhiteboardSession(const ChatListWindow::Chat& chat) {
	beginInsertRows(whiteboardsIndex_, 0, whiteboards_->rowCount());
	whiteboards_->addItem(new ChatListWhiteboardItem(chat, whiteboards_));
	endInsertRows();
}

void ChatListModel::removeWhiteboardSession(const JID& jid) {
	for (int i = 0; i < whiteboards_->rowCount(); i++) {
		ChatListWhiteboardItem* item = dynamic_cast<ChatListWhiteboardItem*>(whiteboards_->item(i));
		if (item->getChat().jid == jid) {
			beginRemoveRows(whiteboardsIndex_, i, i+1);
			whiteboards_->remove(i);
			endRemoveRows();
			break;
		}
	}
}

void ChatListModel::setRecents(const std::list<ChatListWindow::Chat>& recents) {
	beginRemoveRows(recentsIndex_, 0, recents_->rowCount());
	recents_->clear();
	endRemoveRows();
	beginInsertRows(recentsIndex_, 0, recents.size());
	foreach (const ChatListWindow::Chat chat, recents) {
		recents_->addItem(new ChatListRecentItem(chat, recents_));
//whiteboards_->addItem(new ChatListRecentItem(chat, whiteboards_));
	}
	endInsertRows();
}

QMimeData* ChatListModel::mimeData(const QModelIndexList& indexes) const {
	QMimeData* data = QAbstractItemModel::mimeData(indexes);
	ChatListRecentItem *item = dynamic_cast<ChatListRecentItem*>(getItemForIndex(indexes.first()));
	if (item == NULL) {
		return data;
	}

	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	const ChatListWindow::Chat& chat = item->getChat();

	QString mimeType = "application/vnd.swift.contact-jid-list";
	if (!chat.impromptuJIDs.size()) {
		if (chat.isMUC) {
			mimeType = "application/vnd.swift.contact-jid-muc";
		}
		dataStream << P2QSTRING(chat.jid.toString());
	} else {
		typedef std::map<std::string, JID> JIDMap;
		foreach (const JIDMap::value_type& jid, chat.impromptuJIDs) {
			dataStream << P2QSTRING(jid.second.toString());
		}
	}

	data->setData(mimeType, itemData);
	return data;
}

const ChatListMUCItem* ChatListModel::getChatListMUCItem(const JID& roomJID) const {
	const ChatListMUCItem* mucItem = NULL;
	for (int i = 0; i < mucBookmarks_->rowCount(); i++) {
		ChatListMUCItem* item = dynamic_cast<ChatListMUCItem*>(mucBookmarks_->item(i));
		if (item->getBookmark().getRoom() == roomJID) {
			mucItem = item;
			break;
		}
	}
	return mucItem;
}

int ChatListModel::columnCount(const QModelIndex& /*parent*/) const {
	return 1;
}

ChatListItem* ChatListModel::getItemForIndex(const QModelIndex& index) const {
	return index.isValid() ? static_cast<ChatListItem*>(index.internalPointer()) : NULL;
}

QVariant ChatListModel::data(const QModelIndex& index, int role) const {
	ChatListItem* item = getItemForIndex(index);
	return item ? item->data(role) : QVariant();
}

QModelIndex ChatListModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	ChatListGroupItem *parentItem = parent.isValid() ? static_cast<ChatListGroupItem*>(parent.internalPointer()) : root_;

	return row < parentItem->rowCount() ? createIndex(row, column, parentItem->item(row)) : QModelIndex();
}

QModelIndex ChatListModel::parent(const QModelIndex& index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}
	ChatListGroupItem* parent = static_cast<ChatListGroupItem*>(index.internalPointer())->parent();
	return (parent == root_) ? QModelIndex() : createIndex(parent->parent()->row(parent), 0, parent);
}

int ChatListModel::rowCount(const QModelIndex& parentIndex) const {
	ChatListGroupItem* parent = NULL;
	if (parentIndex.isValid()) {
		parent = dynamic_cast<ChatListGroupItem*>(static_cast<ChatListItem*>(parentIndex.internalPointer()));
	} else {
		parent = root_;
	}
	int count = (parent ? parent->rowCount() : 0);
	return count;
}

}
