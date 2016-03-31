/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/MUCSearch/MUCSearchRoomItem.h>

#include <Swift/QtUI/MUCSearch/MUCSearchServiceItem.h>

namespace Swift {
MUCSearchRoomItem::MUCSearchRoomItem(const QString& node, MUCSearchServiceItem* parent) : parent_(parent), node_(node) {
    parent_->addRoom(this);
}

MUCSearchServiceItem* MUCSearchRoomItem::getParent() {
    return parent_;
}
QVariant MUCSearchRoomItem::data(int role) {
    switch (role) {
        case Qt::DisplayRole: return QVariant(node_);
        default: return QVariant();
    }
}

}
