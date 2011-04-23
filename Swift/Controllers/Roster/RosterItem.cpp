/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Roster/RosterItem.h"

#include <boost/algorithm/string.hpp>

#include "Swift/Controllers/Roster/GroupRosterItem.h"

namespace Swift {

RosterItem::RosterItem(const std::string& name, GroupRosterItem* parent) : name_(name), sortableDisplayName_(boost::to_lower_copy(name_)), parent_(parent) {
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

void RosterItem::setDisplayName(const std::string& name) {
	name_ = name;
	sortableDisplayName_ = boost::to_lower_copy(name_);
	onDataChanged();
}

const std::string& RosterItem::getDisplayName() const {
	return name_;
}

const std::string& RosterItem::getSortableDisplayName() const {
	return sortableDisplayName_;
}


}

