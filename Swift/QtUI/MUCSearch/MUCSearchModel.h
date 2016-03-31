/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QAbstractItemModel>
#include <QList>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

namespace Swift {
    class MUCSearchModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            MUCSearchModel();
            void clear();
            void addService(MUCSearchServiceItem* service);
            int columnCount(const QModelIndex& parent = QModelIndex()) const;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
            QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex& index) const;
            int rowCount(const QModelIndex& parent = QModelIndex()) const;
//            ChatListItem* getItemForIndex(const QModelIndex& index) const;
        private:
//            ChatListGroupItem* mucBookmarks_;
//            ChatListGroupItem* root_;
            QList<MUCSearchServiceItem*> services_;
    };

}
