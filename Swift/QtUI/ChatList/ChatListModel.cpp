#include "Swift/QtUI/ChatList/ChatListModel.h"

#include "Swift/QtUI/ChatList/ChatListMUCItem.h"

namespace Swift {

ChatListModel::ChatListModel() {
	root_ = new ChatListGroupItem("", NULL);
	mucBookmarks_ = new ChatListGroupItem("MUC Bookmarks", root_);
}

void ChatListModel::addMUCBookmark(boost::shared_ptr<Swift::MUCBookmark> bookmark) {
	emit layoutAboutToBeChanged();
	mucBookmarks_->addItem(new ChatListMUCItem(bookmark, mucBookmarks_));
	emit layoutChanged();
}

void ChatListModel::removeMUCBookmark(boost::shared_ptr<Swift::MUCBookmark> bookmark) {
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

QVariant ChatListModel::data(const QModelIndex& index, int role) const {
	return index.isValid() ? static_cast<ChatListItem*>(index.internalPointer())->data(role) : QVariant();
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
	ChatListGroupItem* parent = root_;
	if (parentIndex.isValid()) {
		parent = static_cast<ChatListGroupItem*>(parentIndex.internalPointer());
	}
	return parent ? parent->rowCount() : 0;
}

}
