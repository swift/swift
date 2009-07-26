#include "RosterItem.h"

namespace Swift {
	
RosterItem::RosterItem(RosterItem* parent) {
	parent_ = parent;
}

RosterItem* RosterItem::getParentItem() {
	return parent_;
}

void RosterItem::addChild(RosterItem* child) {
	children_.append(child);
}

int RosterItem::rowCount() {
	return children_.size();
}

int RosterItem::rowOf(RosterItem* item) {
	return children_.indexOf(item);;
}
	
RosterItem* RosterItem::getItem(int row) {
	return children_[row];
}
}