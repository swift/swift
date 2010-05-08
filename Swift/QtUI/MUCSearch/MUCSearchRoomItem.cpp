/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/MUCSearch/MUCSearchRoomItem.h"

#include "Swift/QtUI/MUCSearch/MUCSearchServiceItem.h"

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
