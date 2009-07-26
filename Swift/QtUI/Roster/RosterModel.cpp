#include "RosterModel.h"

namespace Swift {

RosterModel::RosterModel(RosterItem* tree) {
	tree_ = tree;
}

RosterModel::~RosterModel() {
	
}

int RosterModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return 1;
}

QVariant RosterModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid()) {
		return QVariant();
	}
	return QVariant("bob");
}

QModelIndex RosterModel::index(int row, int column, const QModelIndex& parent) const {
	RosterItem* parentItem = parent.isValid() ? static_cast<RosterItem*>(parent.internalPointer()) : tree_;
	Q_ASSERT(parentItem);
	
	return row < parentItem->rowCount() ? createIndex(row, column, parentItem->getItem(row)) : QModelIndex();
}

QModelIndex RosterModel::parent(const QModelIndex& index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}
	
	RosterItem* item = static_cast<RosterItem*>(index.internalPointer());
	Q_ASSERT(item);

	RosterItem* parentItem = item->getParentItem();
	return parentItem == tree_ ? QModelIndex() : createIndex(parentItem->getParentItem()->rowOf(parentItem), 0, parentItem);

}

int RosterModel::rowCount(const QModelIndex& parent) const {
	if (!parent.isValid()) {
		return 0;
	}
	
	RosterItem* item = static_cast<RosterItem*>(parent.internalPointer());
	Q_ASSERT(item);
	
	return item->rowCount();
}

}