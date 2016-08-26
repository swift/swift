/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchRoomItem.h>

#include <memory>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

namespace Swift {

MUCSearchRoomItem::MUCSearchRoomItem(const QString& node) : node_(node) {

}

void MUCSearchRoomItem::setParent(std::weak_ptr<MUCSearchServiceItem> parent) {
    parent_ = parent;
}

std::shared_ptr<MUCSearchServiceItem> MUCSearchRoomItem::getParent() {
    return parent_.lock();
}

QVariant MUCSearchRoomItem::data(int role) {
    switch (role) {
        case Qt::DisplayRole: return QVariant(node_);
        default: return QVariant();
    }
}

}
