/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/QtLeafSortFilterProxyModel.h>

namespace Swift {

QtLeafSortFilterProxyModel::QtLeafSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {

}

QtLeafSortFilterProxyModel::~QtLeafSortFilterProxyModel() {

}

bool QtLeafSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    if (!sourceModel()->hasChildren(sourceModel()->index(source_row, 0, source_parent))) {
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }
    else {
        return true;
    }
}

}
