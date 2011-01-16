/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/MUCSearch/MUCSearchModel.h"
#include "Swift/QtUI/MUCSearch/MUCSearchEmptyItem.h"

namespace Swift {

MUCSearchModel::MUCSearchModel() {
}

void MUCSearchModel::clear() {
	emit layoutAboutToBeChanged();
	services_.clear();
	emit layoutChanged();
}

void MUCSearchModel::addService(MUCSearchServiceItem* service) {
	emit layoutAboutToBeChanged();
	services_.push_back(service);
	emit layoutChanged();
}

int MUCSearchModel::columnCount(const QModelIndex& /*parent*/) const {
	return 1;
}

QVariant MUCSearchModel::data(const QModelIndex& index, int role) const {
	return index.isValid() ? static_cast<MUCSearchItem*>(index.internalPointer())->data(role) : QVariant();
}

QModelIndex MUCSearchModel::index(int row, int column, const QModelIndex & parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}
	
	if (parent.isValid()) {
		MUCSearchServiceItem* parentItem = static_cast<MUCSearchServiceItem*>(parent.internalPointer());
		return row < parentItem->rowCount() ? createIndex(row, column, parentItem->getItem(row)) : QModelIndex();
	} else {
		return row < services_.size() ? createIndex(row, column, services_[row]) : QModelIndex();
	}


}

QModelIndex MUCSearchModel::parent(const QModelIndex& index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}
	MUCSearchItem* item = static_cast<MUCSearchItem*>(index.internalPointer());
	if (!item) {
		return QModelIndex();
	}
	else if (dynamic_cast<MUCSearchServiceItem*>(item)) {
		return QModelIndex();
	}

	MUCSearchServiceItem* parent = NULL;
	if (MUCSearchRoomItem* roomItem = dynamic_cast<MUCSearchRoomItem*>(item)) {
		parent = roomItem->getParent();
	}
	else if (MUCSearchEmptyItem* emptyItem = dynamic_cast<MUCSearchEmptyItem*>(item)) {
		parent = emptyItem->getParent();
	}
	if (parent) {
		int row = services_.indexOf(parent);
		return createIndex(row, 1, parent);
	}
	else {
		return QModelIndex();
	}
}

int MUCSearchModel::rowCount(const QModelIndex& parentIndex) const {
	if (!parentIndex.isValid()) {
		return services_.size();	
	} 
	if (dynamic_cast<MUCSearchServiceItem*>(static_cast<MUCSearchItem*>(parentIndex.internalPointer()))) {
		return services_[parentIndex.row()]->rowCount();
	}
	else {
		return 0;
	}
}

}
