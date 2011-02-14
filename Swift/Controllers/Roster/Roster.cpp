/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Roster/Roster.h"

#include "Swiften/Base/foreach.h"
#include <string>
#include "Swiften/JID/JID.h"
#include "Swift/Controllers/Roster/ContactRosterItem.h"
#include "Swift/Controllers/Roster/RosterItem.h"
#include "Swift/Controllers/Roster/GroupRosterItem.h"
#include "Swift/Controllers/Roster/RosterItemOperation.h"

#include <boost/bind.hpp>

#include <iostream>
#include <deque>

namespace Swift {

Roster::Roster(bool sortByStatus, bool fullJIDMapping) {
	sortByStatus_ = sortByStatus;
	fullJIDMapping_ = fullJIDMapping;
	root_ = new GroupRosterItem("Dummy-Root", NULL, sortByStatus_);
	root_->onChildrenChanged.connect(boost::bind(&Roster::handleChildrenChanged, this, root_));
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

GroupRosterItem* Roster::getGroup(const std::string& groupName) {
	foreach (RosterItem *item, root_->getChildren()) {
		GroupRosterItem *group = dynamic_cast<GroupRosterItem*>(item);
		if (group && group->getDisplayName() == groupName) {
			return group;
		}
	}
	GroupRosterItem* group = new GroupRosterItem(groupName, root_, sortByStatus_);
	root_->addChild(group);
	group->onChildrenChanged.connect(boost::bind(&Roster::handleChildrenChanged, this, group));
	group->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, group));
	return group;
}

void Roster::removeGroup(const std::string& group) {
	root_->removeGroupChild(group);
}

void Roster::handleDataChanged(RosterItem* item) {
	onDataChanged(item);
}

void Roster::handleChildrenChanged(GroupRosterItem* item) {
	onChildrenChanged(item);
}

void Roster::addContact(const JID& jid, const JID& displayJID, const std::string& name, const std::string& groupName, const std::string& avatarPath) {
	GroupRosterItem* group(getGroup(groupName));
	ContactRosterItem *item = new ContactRosterItem(jid, displayJID, name, group);
	item->setAvatarPath(avatarPath);
	group->addChild(item);
	if (itemMap_[fullJIDMapping_ ? jid : jid.toBare()].size() > 0) {
		foreach (std::string existingGroup, itemMap_[fullJIDMapping_ ? jid : jid.toBare()][0]->getGroups()) {
			item->addGroup(existingGroup);
		}
	}
	itemMap_[fullJIDMapping_ ? jid : jid.toBare()].push_back(item);
	item->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, item));
	filterContact(item, group);

	foreach (ContactRosterItem* item, itemMap_[fullJIDMapping_ ? jid : jid.toBare()]) {
		item->addGroup(groupName);
	}
}

struct JIDEqualsTo {
	JIDEqualsTo(const JID& jid) : jid(jid) {}
	bool operator()(ContactRosterItem* i) const { return jid == i->getJID(); }
	JID jid;
};

void Roster::removeAll() {
	root_->removeAll();
	itemMap_.clear();
	onChildrenChanged(root_);
	onDataChanged(root_);
}

void Roster::removeContact(const JID& jid) {
	std::vector<ContactRosterItem*>* items = &itemMap_[fullJIDMapping_ ? jid : jid.toBare()];
	items->erase(std::remove_if(items->begin(), items->end(), JIDEqualsTo(jid)), items->end());
	if (items->size() == 0) {
		itemMap_.erase(fullJIDMapping_ ? jid : jid.toBare());
	}
	//Causes the delete
	root_->removeChild(jid);
}

void Roster::removeContactFromGroup(const JID& jid, const std::string& groupName) {
	std::vector<RosterItem*> children = root_->getChildren();
	std::vector<RosterItem*>::iterator it = children.begin();
	while (it != children.end()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group && group->getDisplayName() == groupName) {
			ContactRosterItem* deleted = group->removeChild(jid);
			std::vector<ContactRosterItem*>* items = &itemMap_[fullJIDMapping_ ? jid : jid.toBare()];
			items->erase(std::remove(items->begin(), items->end(), deleted), items->end());
		}
		it++;
	}
	foreach (ContactRosterItem* item, itemMap_[fullJIDMapping_ ? jid : jid.toBare()]) {
		item->removeGroup(groupName);
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
	
	foreach (ContactRosterItem* item, itemMap_[fullJIDMapping_ ? jid : jid.toBare()]) {
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
	if (oldDisplayedSize == 0 && newDisplayedSize > 0) {
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

