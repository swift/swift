/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

#include <memory>

#include <QString>
#include <QVector>
#include <QtAlgorithms>

namespace Swift {

MUCSearchServiceItem::MUCSearchServiceItem(const QString& jidString) : jidString_(jidString) {

}

void MUCSearchServiceItem::addRoom(std::shared_ptr<MUCSearchItem> room) {
    room->setParent(shared_from_this());
    rooms_.push_back(room);
    if (sortOrder_) {
        sort();
    }
}

void MUCSearchServiceItem::addRooms(const std::vector<std::shared_ptr<MUCSearchItem> >& rooms) {
    for (auto&& room: rooms) {
        room->setParent(shared_from_this());
        rooms_.push_back(room);
    }
    if (sortOrder_) {
        sort();
    }
}

int MUCSearchServiceItem::rowCount() {
    return rooms_.count();
}

MUCSearchItem* MUCSearchServiceItem::getItem(int i) {
    return rooms_[i].get();
}

QVariant MUCSearchServiceItem::data(int role) {
    switch (role) {
        case Qt::DisplayRole:
            return QVariant(jidString_);
        default:
            return QVariant();
    }
}

QString MUCSearchServiceItem::getHost() const {
    return jidString_;
}

void MUCSearchServiceItem::setSorting(Qt::SortOrder sortOrder) {
    sortOrder_ = sortOrder;
    sort();
}

void MUCSearchServiceItem::sort() {
    if (*sortOrder_ == Qt::AscendingOrder) {
        qStableSort(rooms_.begin(), rooms_.end(), [](const std::shared_ptr<MUCSearchItem>& item1, const std::shared_ptr<MUCSearchItem>& item2) -> bool {
            return QString::localeAwareCompare(item1->data(Qt::DisplayRole).toString(), item2->data(Qt::DisplayRole).toString()) < 0;
        });
    }
    else {
        qStableSort(rooms_.begin(), rooms_.end(), [](const std::shared_ptr<MUCSearchItem>& item1, const std::shared_ptr<MUCSearchItem>& item2) -> bool {
            return QString::localeAwareCompare(item1->data(Qt::DisplayRole).toString(), item2->data(Qt::DisplayRole).toString()) > 0;
        });
    }
}

}
