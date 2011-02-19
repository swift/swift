/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ChatList/ChatListModel.h"

#include "Swift/QtUI/ChatList/ChatListMUCItem.h"

namespace Swift {

ChatListModel::ChatListModel() {
	root_ = new ChatListGroupItem("", NULL);
	mucBookmarks_ = new ChatListGroupItem(tr("Bookmarked Rooms"), root_);
	root_->addItem(mucBookmarks_);
}

void ChatListModel::clear() {
	emit layoutAboutToBeChanged();
	mucBookmarks_->clear();
	emit layoutChanged();
}

void ChatListModel::addMUCBookmark(const Swift::MUCBookmark& bookmark) {
	emit layoutAboutToBeChanged();
	mucBookmarks_->addItem(new ChatListMUCItem(bookmark, mucBookmarks_));
	emit layoutChanged();
	//QModelIndex index = createIndex(mucBookmarks_->rowCount() - 1, 0, mucBookmarks_);
	//emit dataChanged(index, index);
	//emit dataChanged(parent(index), parent(index));
}

void ChatListModel::removeMUCBookmark(const Swift::MUCBookmark& bookmark) {
	for (int i = 0; i < mucBookmarks_->rowCount(); i++) {
		ChatListMUCItem* item = dynamic_cast<ChatListMUCItem*>(mucBookmarks_->item(i));
		if (item->getBookmark() == bookmark) {
			emit layoutAboutToBeChanged();
			mucBookmarks_->remove(i);
			emit layoutChanged();
			break;
		}
	}
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
