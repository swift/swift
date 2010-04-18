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
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetFactory.h"

#include <boost/bind.hpp>

#include <deque>

namespace Swift {

Roster::Roster(TreeWidget *treeWidget, TreeWidgetFactory *widgetFactory) : treeWidget_(treeWidget), widgetFactory_(widgetFactory) {
}

Roster::~Roster() {
	foreach (RosterItem* item, items_) {
		delete item;
	}
	delete treeWidget_;
}

TreeWidget* Roster::getWidget() {
	return treeWidget_;
}

GroupRosterItem* Roster::getGroup(const String& groupName) {
	foreach (RosterItem *item, children_) {
		GroupRosterItem *group = dynamic_cast<GroupRosterItem*>(item);
		if (group && group->getName() == groupName) {
			return group;
		}
	}
	GroupRosterItem* group = new GroupRosterItem(groupName, treeWidget_, widgetFactory_);
	children_.push_back(group);
	items_.push_back(group);
	return group;
}

void Roster::handleUserAction(boost::shared_ptr<UserRosterAction> action) {
	onUserAction(action);
}

void Roster::addContact(const JID& jid, const String& name, const String& group) {
	ContactRosterItem *item = new ContactRosterItem(jid, name, getGroup(group), widgetFactory_);
	items_.push_back(item);
	itemMap_[jid.toBare()].push_back(item);
	item->onUserAction.connect(boost::bind(&Roster::handleUserAction, this, _1));
	filterItem(item);

}

void Roster::removeContact(const JID& jid) {
	itemMap_.erase(jid.toBare());
	std::vector<RosterItem*>::iterator it = children_.begin();
	while (it != children_.end()) {
		ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(*it);
		if (contact && contact->getJID() == jid) {
			delete contact;
			it = children_.erase(it);
			continue;
		} 
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group) {
			group->removeChild(jid);
		}
		it++;
	}
}

void Roster::removeContactFromGroup(const JID& jid, const String& groupName) {
	std::vector<RosterItem*>::iterator it = children_.begin();
	while (it != children_.end()) {
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
		if (group && group->getName() == groupName) {
			group->removeChild(jid);
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
	foreach (RosterItem* item, itemMap_[jid]) {
		operation(item);
		filterItem(item);
	}
}

void Roster::applyOnAllItems(const RosterItemOperation& operation) {
	std::deque<RosterItem*> queue(children_.begin(), children_.end());
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


void Roster::filterItem(RosterItem* rosterItem) {
	ContactRosterItem *item = dynamic_cast<ContactRosterItem*>(rosterItem);
	if (!item) {
		return;
	}
	bool hide = true;
	foreach (RosterFilter *filter, filters_) {
		hide &= (*filter)(item);
	}
	filters_.size() > 0 && hide ? item->hide() : item->show();
}

void Roster::filterAll() {
	std::deque<RosterItem*> queue(children_.begin(), children_.end());
	while (!queue.empty()) {
		RosterItem *item = *queue.begin();
		queue.pop_front();
		GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
		if (group) {
			queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
		} else {
			filterItem(item);
		}
	}
}

}

