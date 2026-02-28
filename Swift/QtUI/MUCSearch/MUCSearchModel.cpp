/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchModel.h>

#include <memory>

#include <Swift/QtUI/MUCSearch/MUCSearchEmptyItem.h>

namespace Swift {

MUCSearchModel::MUCSearchModel() {
}

void MUCSearchModel::clear() {
    // We need to reset the model, so that model indices containing raw pointers
    // to MUCSearchServiceItems are invalaidated before we delete the
    // MUCSearchServiceItems.
    emit beginResetModel();
    services_.clear();
    emit endResetModel();
}

void MUCSearchModel::addService(std::shared_ptr<MUCSearchServiceItem> service) {
    emit layoutAboutToBeChanged();
    if (sortOrder_) {
        service->setSorting(*sortOrder_);
    }
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
        return row < services_.size() ? createIndex(row, column, services_[row].get()) : QModelIndex();
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

    std::shared_ptr<MUCSearchServiceItem> parent;
    if (MUCSearchRoomItem* roomItem = dynamic_cast<MUCSearchRoomItem*>(item)) {
        parent = roomItem->getParent();
    }
    else if (MUCSearchEmptyItem* emptyItem = dynamic_cast<MUCSearchEmptyItem*>(item)) {
        parent = emptyItem->getParent();
    }
    if (parent) {
        int row = services_.indexOf(parent);
        return createIndex(row, 1, parent.get());
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

void MUCSearchModel::sort(int column, Qt::SortOrder order) {
    sortOrder_ = order;
    if (column == 0) {
        for (auto&& serviceItem : services_) {
            serviceItem->setSorting(*sortOrder_);
        }
    }
}

}
