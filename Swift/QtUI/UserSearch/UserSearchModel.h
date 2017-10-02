/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QAbstractItemModel>
#include <QList>

#include <Swift/Controllers/Chat/UserSearchController.h>

namespace Swift {
    class UserSearchModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            enum UserItemRoles {
                DetailTextRole = Qt::UserRole/*,
                AvatarRole = Qt::UserRole + 1,
                PresenceIconRole = Qt::UserRole + 2,
                StatusShowTypeRole = Qt::UserRole + 3*/
            };
            UserSearchModel();
            void clear();
            void setResults(const std::vector<UserSearchResult>& results);
            int columnCount(const QModelIndex& parent = QModelIndex()) const;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
            static QVariant data(UserSearchResult* item, int role);
            QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex& index) const;
            int rowCount(const QModelIndex& parent = QModelIndex()) const;
        private:
            static QString nameLine(UserSearchResult* item);
            static QString detailLine(UserSearchResult* item);
            std::vector<UserSearchResult> results_;
    };

}
