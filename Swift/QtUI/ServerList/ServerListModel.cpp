/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

//#include <Swift/QtUI/ServerList/ServerListModel.h>
#include <QBrush>
#include <QColor>
#include <QMimeData>

#include "ServerListModel.h"

namespace Swift {

ServerListModel::ServerListModel() {
}

ServerListModel::~ServerListModel() {
}

QVariant ServerListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    SwiftAccountData::SwiftAccountDataItem* item = static_cast<SwiftAccountData::SwiftAccountDataItem*>(index.internalPointer());
    switch (role) {
    case Qt::DisplayRole: return QString(item->userJID_.toBare().toString().c_str());
    case Qt::BackgroundRole: return QBrush(Qt::transparent);
    case Qt::ToolTipRole: return QString(item->userJID_.toBare().toString().c_str());
    default: return QVariant();
    }
}

QModelIndex ServerListModel::index(int row, int column, const QModelIndex& /*parent*/) const {
    if (!modelData_ || static_cast<size_t>(row) >= modelData_->size()) {
        return QModelIndex();
    }
    return createIndex(row, column, modelData_->getAccount(row));
}

QModelIndex ServerListModel::parent(const QModelIndex& /*index*/) const {
    return QModelIndex();
}

QMimeData* ServerListModel::mimeData(const QModelIndexList& indexes) const {
    return QAbstractItemModel::mimeData(indexes);
}

int ServerListModel::rowCount(const QModelIndex& /*parent*/) const {
    if (!modelData_) {
        return 0;
    }
    return modelData_->size();
}

int ServerListModel::columnCount(const QModelIndex& /*parent*/) const {
    if (!modelData_) {
        return 0;
    }
    return 1;
}

void ServerListModel::handleDataChanged() {
    emit layoutChanged();
}

}
