/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Roster/GroupRosterItem.h"

#include <boost/bind.hpp>
//#include <boost/algorithm.hpp>
#include <iostream>

namespace Swift {

GroupRosterItem::GroupRosterItem(const std::string& name, GroupRosterItem* parent, bool sortByStatus) : RosterItem(name, parent), sortByStatus_(sortByStatus), manualSort_(false) {
	expanded_ = true;
}

GroupRosterItem::~GroupRosterItem() {

}

void GroupRosterItem::setManualSort(const std::string& manualSortValue) {
	manualSort_ = true;
	bool changed = manualSortValue_ != manualSortValue;
	manualSortValue_ = manualSortValue;
	if (changed) {
		onChildrenChanged();
		onDataChanged();
	}
}

const std::string& GroupRosterItem::getSortableDisplayName() const {
	return manualSort_ ? manualSortValue_ : RosterItem::getSortableDisplayName();
}

bool GroupRosterItem::isExpanded() const {
	return expanded_;
}

/**
	This has no effect, and is only used by the UI.
	If reTransmit is specified, dataChanged will be emitted on a change -
	This may be undesireable if called from the UI, so you can use reTransmit=false
	to avoid a loop in this case.
 */
void GroupRosterItem::setExpanded(bool expanded) {
	bool old = expanded_;
	expanded_ = expanded;
	if (expanded != old) {
		onExpandedChanged(expanded);
	}
}

const std::vector<RosterItem*>& GroupRosterItem::getChildren() const {
	return children_;
}

const std::vector<RosterItem*>& GroupRosterItem::getDisplayedChildren() const {
	return displayedChildren_;
}

void GroupRosterItem::addChild(RosterItem* item) {
	children_.push_back(item);
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	if (group) {
		group->onChildrenChanged.connect(boost::bind(&GroupRosterItem::handleChildrenChanged, this, group));
	} else {
		item->onDataChanged.connect(boost::bind(&GroupRosterItem::handleDataChanged, this, item));
	}
	onChildrenChanged();
	onDataChanged();
}

/**
 * Does not emit a changed signal.
 */
void GroupRosterItem::removeAll() {
	std::vector<RosterItem*>::iterator it = children_.begin();
	displayedChildren_.clear();
	while (it != children_.end()) {
		ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(*it);
		if (contact) {
			delete contact;
		} else {
			GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
			if (group) {
				group->removeAll();
				delete group;
			}
		}
		++it;
	}
	children_.clear();
}

/**
 * Returns the removed item - but only if it's the only one, otherwise
 * the return result is undefined.
 */
ContactRosterItem* GroupRosterItem::removeChild(const JID& jid) {
	std::vector<RosterItem*>::iterator it = children_.begin();
	ContactRosterItem* removed = NULL;
	while (it != children_.end()) {
		ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(*it);
		if (contact && contact->getJID() == jid) {
			displayedChildren_.erase(std::remove(displayedChildren_.begin(), displayedChildren_.end(), contact), displayedChildren_.end());
			removed = contact;
			delete contact;
			it = children_.erase(it);
			continue;
		} 
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group) {
			ContactRosterItem* groupRemoved = group->removeChild(jid);
			if (groupRemoved) {
				removed = groupRemoved;
			}
		}
		++it;
	}
	onChildrenChanged();
	onDataChanged();
	return removed;
}

GroupRosterItem* GroupRosterItem::removeGroupChild(const std::string& groupName) {
	std::vector<RosterItem*>::iterator it = children_.begin();
	GroupRosterItem* removed = NULL;
	while (it != children_.end()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group && group->getDisplayName() == groupName) {
			displayedChildren_.erase(std::remove(displayedChildren_.begin(), displayedChildren_.end(), group), displayedChildren_.end());
			removed = group;
			delete group;
			it = children_.erase(it);
			continue;
		}
		++it;
	}
	onChildrenChanged();
	onDataChanged();
	return removed;
}

/**
 * Returns false if the list didn't need a resort
 */
bool GroupRosterItem::sortDisplayed() {
	/* Not doing this until we import boost::algorithm*/
//	if (boost::is_sorted(displayedChildren_begin(), displayedChildren_.end(), itemLessThan)) {
//		return false;
//	}
	//Sholudn't need stable_sort here
	std::sort(displayedChildren_.begin(), displayedChildren_.end(), sortByStatus_? itemLessThanWithStatus : itemLessThanWithoutStatus);
	return true;
}

bool GroupRosterItem::itemLessThanWithoutStatus(const RosterItem* left, const RosterItem* right) {
	return left->getSortableDisplayName() < right->getSortableDisplayName();
}

bool GroupRosterItem::itemLessThanWithStatus(const RosterItem* left, const RosterItem* right) {
	const ContactRosterItem* leftContact = dynamic_cast<const ContactRosterItem*>(left);
	const ContactRosterItem* rightContact = dynamic_cast<const ContactRosterItem*>(right);
	if (leftContact) {
		if (!rightContact) {
			return false;
		}
		StatusShow::Type leftType = leftContact->getSimplifiedStatusShow();
		StatusShow::Type rightType = rightContact->getSimplifiedStatusShow();
		if (leftType == rightType) {
			return left->getSortableDisplayName() < right->getSortableDisplayName();
		} else {
			return leftType < rightType;
		}
	} else {
		if (rightContact) {
			return true;
		}
		return left->getSortableDisplayName() < right->getSortableDisplayName();
	}
}

void GroupRosterItem::setDisplayed(RosterItem* item, bool displayed) {
	bool found = false;
	for (size_t i = 0; i < displayedChildren_.size(); i++) {
		if (displayedChildren_[i] == item) {
			found = true;
		}
	}
	if (found == displayed) {
		return;
	}
	if (displayed) {
		displayedChildren_.push_back(item);
		sortDisplayed();
	} else {
		displayedChildren_.erase(std::remove(displayedChildren_.begin(), displayedChildren_.end(), item), displayedChildren_.end());
	}
	onChildrenChanged();
	onDataChanged();
}

void GroupRosterItem::handleDataChanged(RosterItem* /*item*/) {
	if (sortDisplayed()) {
		onChildrenChanged();
	}
}

void GroupRosterItem::handleChildrenChanged(GroupRosterItem* group) {
	size_t oldSize = getDisplayedChildren().size();
	if (group->getDisplayedChildren().size() > 0) {
		bool found = false;
		for (size_t i = 0; i < displayedChildren_.size(); i++) {
			if (displayedChildren_[i] == group) {
				found = true;
			}
		}
		if (!found) {
			displayedChildren_.push_back(group);
			sortDisplayed();
		}
	} else {
		displayedChildren_.erase(std::remove(displayedChildren_.begin(), displayedChildren_.end(), group), displayedChildren_.end());
	}

	if (oldSize != getDisplayedChildren().size() || sortDisplayed()) {
		onChildrenChanged();
		onDataChanged();
	}
}


}
