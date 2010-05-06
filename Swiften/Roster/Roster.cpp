/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/Roster.h"

#include "Swiften/Base/foreach.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Roster/GroupRosterItem.h"
#include "Swiften/Roster/RosterItemOperation.h"

#include <boost/bind.hpp>

#include <iostream>
#include <deque>

namespace Swift {

Roster::Roster() {
	root_ = new GroupRosterItem("Dummy-Root", NULL);
}

Roster::~Roster() {
	std::deque<RosterItem*> queue;
	queue.push_back(root_);
	while (!queue.empty()) {
		RosterItem* item = *queue.begin();
		queue.pop_front();
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
		}
		delete item;
	}
}

GroupRosterItem* Roster::getRoot() {
	return root_;
}

GroupRosterItem* Roster::getGroup(const String& groupName) {
	foreach (RosterItem *item, root_->getChildren()) {
		GroupRosterItem *group = dynamic_cast<GroupRosterItem*>(item);
		if (group && group->getDisplayName() == groupName) {
			return group;
		}
	}
	GroupRosterItem* group = new GroupRosterItem(groupName, root_);
	root_->addChild(group);
//	std::cout << "Added " << groupName << " to root" << std::endl;
	group->onChildrenChanged.connect(boost::bind(&Roster::handleChildrenChanged, this, group));
	group->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, group));
	return group;
}

void Roster::handleDataChanged(RosterItem* item) {
	onDataChanged(item);
}

void Roster::handleChildrenChanged(GroupRosterItem* item) {
	onChildrenChanged(item);
}

void Roster::addContact(const JID& jid, const String& name, const String& groupName) {
	GroupRosterItem* group(getGroup(groupName));
	ContactRosterItem *item = new ContactRosterItem(jid, name, group);
	group->addChild(item);
	itemMap_[jid.toBare()].push_back(item);
	item->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, item));
	filterContact(item, group);
}


void Roster::removeContact(const JID& jid) {
	std::vector<ContactRosterItem*> items = itemMap_[jid.toBare()];
	std::vector<ContactRosterItem*>::iterator it = items.begin();
	while (it != items.end()) {
		if (jid == (*it)->getJID()) {
			it = items.erase(it);
		}
	}
	if (items.size() == 0) {
		itemMap_.erase(jid.toBare());
	}
	//Causes the delete
	root_->removeChild(jid);
}

void Roster::removeContactFromGroup(const JID& jid, const String& groupName) {
	std::vector<RosterItem*> children = root_->getChildren();
	std::vector<RosterItem*>::iterator it = children.begin();
	while (it != children.end()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group && group->getDisplayName() == groupName) {
			ContactRosterItem* deleted = group->removeChild(jid);
			std::vector<ContactRosterItem*> items = itemMap_[jid.toBare()];
			items.erase(std::remove(items.begin(), items.end(), deleted), items.end());
		}
		it++;
	}
}


void Roster::applyOnItems(const RosterItemOperation& operation) {
	if (operation.requiresLookup()) {
		applyOnItem(operation, operation.lookupJID());
	} else {
		applyOnAllItems(operation);
	}
}

void Roster::applyOnItem(const RosterItemOperation& operation, const JID& jid) {
	foreach (ContactRosterItem* item, itemMap_[jid.toBare()]) {
		operation(item);
		filterContact(item, item->getParent());
	}
}

void Roster::applyOnAllItems(const RosterItemOperation& operation) {
	std::deque<RosterItem*> queue;
	queue.push_back(root_);
	while (!queue.empty()) {
		RosterItem* item = *queue.begin();
		queue.pop_front();
		operation(item);
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
		}
	}
	filterAll();
}

void Roster::removeFilter(RosterFilter *filter) {
	for (unsigned int i = 0; i < filters_.size(); i++) {
		if (filters_[i] == filter) {
			filters_.erase(filters_.begin() + i);
			break;
		}
	}
	filterAll();
}

void Roster::filterContact(ContactRosterItem* contact, GroupRosterItem* group) {
	int oldDisplayedSize = group->getDisplayedChildren().size();
	bool hide = true;
	foreach (RosterFilter *filter, filters_) {
		hide &= (*filter)(contact);
	}
	group->setDisplayed(contact, filters_.size() == 0 || !hide);
	int newDisplayedSize = group->getDisplayedChildren().size();
//	std::cout << ", new size = " << newDisplayedSize << std::endl;
	if (oldDisplayedSize == 0 && newDisplayedSize > 0) {
//		std::cout << "Newly created" << std::endl;
		onGroupAdded(group);
	}
}

void Roster::filterGroup(GroupRosterItem* group) {
	foreach (RosterItem* child, group->getChildren()) {
		ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(child);
		if (contact) {
			filterContact(contact, group);
		}
	}
}

void Roster::filterAll() {
	std::deque<RosterItem*> queue;
	queue.push_back(root_);
	while (!queue.empty()) {
		RosterItem *item = *queue.begin();
		queue.pop_front();
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
			filterGroup(group);
		}
	}
}

}

