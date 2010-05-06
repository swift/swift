/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/RosterItem.h"

#include "Swiften/Roster/GroupRosterItem.h"

namespace Swift {

RosterItem::RosterItem(const String& name, GroupRosterItem* parent) : name_(name), parent_(parent) {
	/* The following would be good, but because of C++'s inheritance not working in constructors, it's not going to work. */
	//if (parent) {
	//	parent_->addChild(this);
	//}
}

RosterItem::~RosterItem() {

}

GroupRosterItem* RosterItem::getParent() const {
	return parent_;
}

void RosterItem::setDisplayName(const String& name) {
	name_ = name;
	sortableDisplayName_ = name_.getLowerCase();
	onDataChanged();
}

String RosterItem::getDisplayName() const {
	return name_;
}

String RosterItem::getSortableDisplayName() const {
	return sortableDisplayName_;
}


}

