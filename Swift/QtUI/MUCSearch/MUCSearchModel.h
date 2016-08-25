/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <QAbstractItemModel>
#include <QVector>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

namespace Swift {
    class MUCSearchModel : public QAbstractItemModel {
        Q_OBJECT
        public:
            MUCSearchModel();
            void clear();
            void addService(std::shared_ptr<MUCSearchServiceItem> service);
            int columnCount(const QModelIndex& parent = QModelIndex()) const;
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
            QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
            QModelIndex parent(const QModelIndex& index) const;
            int rowCount(const QModelIndex& parent = QModelIndex()) const;
            virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

        private:
            QVector<std::shared_ptr<MUCSearchServiceItem>> services_;
            boost::optional<Qt::SortOrder> sortOrder_;
    };

}
