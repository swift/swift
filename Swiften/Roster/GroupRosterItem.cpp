/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/GroupRosterItem.h"

#include <boost/bind.hpp>
#include <iostream>

namespace Swift {

GroupRosterItem::GroupRosterItem(const String& name, GroupRosterItem* parent) : RosterItem(name, parent) {

}

GroupRosterItem::~GroupRosterItem() {

}

const std::vector<RosterItem*>& GroupRosterItem::getChildren() const {
	return children_;
}

const std::vector<RosterItem*>& GroupRosterItem::getDisplayedChildren() const {
//	std::cout << "Fetching displayed children for " << getDisplayName() << " and found " << displayedChildren_.size() << std::endl;
	return displayedChildren_;
}

void GroupRosterItem::addChild(RosterItem* item) {
	children_.push_back(item);
	GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
	if (group) {
		group->onChildrenChanged.connect(boost::bind(&GroupRosterItem::handleChildrenChanged, this, group));
	}
	onChildrenChanged();
	onDataChanged();
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
		it++;
	}
	onChildrenChanged();
	onDataChanged();
	return removed;
}

void GroupRosterItem::sortDisplayed() {
	std::stable_sort(displayedChildren_.begin(), displayedChildren_.end(), itemLessThan);
}

bool GroupRosterItem::itemLessThan(const RosterItem* left, const RosterItem* right) {
	const ContactRosterItem* leftContact = dynamic_cast<const ContactRosterItem*>(left);
	const ContactRosterItem* rightContact = dynamic_cast<const ContactRosterItem*>(right);
	if (leftContact) {
		if (rightContact) {
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
	onDataChanged();
	onChildrenChanged();
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
	if (oldSize != getDisplayedChildren().size()) {
		onDataChanged();
		onChildrenChanged();
	}
}


}
