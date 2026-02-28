/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/signals2/connection.hpp>

#include <QAbstractListModel>

namespace Swift {
class Chattables;
class ChattablesModel : public QAbstractListModel {
    public:
        enum ChattablesRoles {
            UnreadCountRole = Qt::UserRole,
            TypeRole = Qt::UserRole + 1,
            StatusRole = Qt::UserRole + 2,
            JIDRole = Qt::UserRole + 3
        };
        ChattablesModel(Chattables& chattables, QObject* parent);
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    private:
        Chattables& chattables_;
        std::vector<std::unique_ptr<boost::signals2::scoped_connection>> connectionList_;
};

}
