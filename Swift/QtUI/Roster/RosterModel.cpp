#include "RosterModel.h"

namespace Swift {

RosterModel::RosterModel() {
}

RosterModel::~RosterModel() {
	delete tree_;
}

void RosterModel::setRoot(RosterItem* root) {
	tree_ = root;
	connect(tree_, SIGNAL(changed()), this, SLOT(handleItemChanged()));
}
							  

void RosterModel::handleItemChanged() {
	//FIXME: This is just a lazy hack to cause the view to refresh until it works.
	// Then I'll replace it with the proper implementation.
	printf("Changed\n");
	reset();
	emit layoutChanged();
}

int RosterModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return 1;
}

QVariant RosterModel::data(const QModelIndex& index, int role) const {
	RosterItem* item = index.isValid() ? static_cast<RosterItem*>(index.internalPointer()) : NULL;
	return item ? item->data(role) : QVariant();
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
	return parentItem == tree_ ? QModelIndex() : createIndex(parentItem->row(), 0, parentItem);

}

int RosterModel::rowCount(const QModelIndex& parent) const {
	RosterItem* item = parent.isValid() ? static_cast<RosterItem*>(parent.internalPointer()) : tree_;
	Q_ASSERT(item);
	
	return item->rowCount();
}

}