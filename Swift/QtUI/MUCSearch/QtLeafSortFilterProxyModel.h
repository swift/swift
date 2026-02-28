/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QSortFilterProxyModel>

namespace Swift {

/**
 * @brief The QtLeafSortFilterProxyModel class is similar to the QSortFilterProxyModel
 * class. While the basic QSortFilterProxyModel class filters all hierarchical items,
 * the QtLeafSortFilterProxyModel class will only filter on items without children.
 */
class QtLeafSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    QtLeafSortFilterProxyModel(QObject* parent = nullptr);
    virtual ~QtLeafSortFilterProxyModel();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
};

}
