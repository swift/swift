/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <QString>
#include <QVector>

#include <Swift/QtUI/MUCSearch/MUCSearchRoomItem.h>

namespace Swift {
    class MUCSearchServiceItem : public MUCSearchItem, public std::enable_shared_from_this<MUCSearchServiceItem> {
        public:
            MUCSearchServiceItem(const QString& jidString);

            void addRoom(std::shared_ptr<MUCSearchItem> room);
            void addRooms(const std::vector<std::shared_ptr<MUCSearchItem>>& rooms);
            int rowCount();
            MUCSearchItem* getItem(int i);
            QVariant data(int role);
            QString getHost() const;
            void setSorting(Qt::SortOrder sortOrder);

        private:
            void sort();

        private:
            QVector<std::shared_ptr<MUCSearchItem>> rooms_;
            QString jidString_;
            boost::optional<Qt::SortOrder> sortOrder_;
    };
}
