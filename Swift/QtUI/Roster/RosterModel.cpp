#include "RosterModel.h"

namespace Swift {

RosterModel::RosterModel() {
}

RosterModel::~RosterModel() {
	delete tree_;
}

void RosterModel::setRoot(QtTreeWidgetItem* root) {
	tree_ = root;
	connect(tree_, SIGNAL(changed(QtTreeWidgetItem*)), this, SLOT(handleItemChanged(QtTreeWidgetItem*)));
}
							  

void RosterModel::handleItemChanged(QtTreeWidgetItem* item) {
	if (!item->isShown()) {
		return;
	}
	Q_ASSERT(item);
	QModelIndex modelIndex = index(item);
	Q_ASSERT(modelIndex.isValid());
	emit itemExpanded(modelIndex, item->isExpanded());
	emit dataChanged(modelIndex, modelIndex);
	emit layoutChanged();
}

int RosterModel::columnCount(const QModelIndex& parent) const {
	Q_UNUSED(parent);
	return 1;
}

QVariant RosterModel::data(const QModelIndex& index, int role) const {
	QtTreeWidgetItem* item = index.isValid() ? static_cast<QtTreeWidgetItem*>(index.internalPointer()) : NULL;
	return item ? item->data(role) : QVariant();
}

QModelIndex RosterModel::index(int row, int column, const QModelIndex& parent) const {
	QtTreeWidgetItem* parentItem = parent.isValid() ? static_cast<QtTreeWidgetItem*>(parent.internalPointer()) : tree_;
	Q_ASSERT(parentItem);
	
	return row < parentItem->rowCount() ? createIndex(row, column, parentItem->getItem(row)) : QModelIndex();
}

QModelIndex RosterModel::index(QtTreeWidgetItem* item) const {
	return createIndex(item->row(), 0, item);
}

QModelIndex RosterModel::parent(const QModelIndex& index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}
	
	QtTreeWidgetItem* item = static_cast<QtTreeWidgetItem*>(index.internalPointer());
	Q_ASSERT(item);

	QtTreeWidgetItem* parentItem = item->getParentItem();
	return parentItem == tree_ ? QModelIndex() : createIndex(parentItem->row(), 0, parentItem);

}

int RosterModel::rowCount(const QModelIndex& parent) const {
	QtTreeWidgetItem* item = parent.isValid() ? static_cast<QtTreeWidgetItem*>(parent.internalPointer()) : tree_;
	Q_ASSERT(item);
	
	return item->rowCount();
}

}