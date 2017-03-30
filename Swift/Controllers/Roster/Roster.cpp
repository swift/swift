/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Roster/Roster.h>

#include <algorithm>
#include <deque>
#include <memory>
#include <set>
#include <string>

#include <boost/bind.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>
#include <Swift/Controllers/Roster/RosterItem.h>

namespace Swift {

Roster::Roster(bool sortByStatus, bool fullJIDMapping) : fullJIDMapping_(fullJIDMapping), sortByStatus_(sortByStatus), root_(std::unique_ptr<GroupRosterItem>(new GroupRosterItem("Dummy-Root", nullptr, sortByStatus_))) {
    root_->onChildrenChanged.connect(boost::bind(&Roster::handleChildrenChanged, this, root_.get()));
}

Roster::~Roster() {
    std::deque<RosterItem*> queue;
    while (!queue.empty()) {
        RosterItem* item = *queue.begin();
        queue.pop_front();
        GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
        if (group) {
            queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
        }
        ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
        if (contact) {
            contact->onVCardRequested.disconnect(boost::bind(boost::ref(onVCardUpdateRequested), contact->getJID()));
        }
        delete item;
    }
}

GroupRosterItem* Roster::getRoot() const {
    return root_.get();
}

std::set<JID> Roster::getJIDs() const {
    std::set<JID> jids;

    std::deque<RosterItem*> queue;
    queue.push_back(root_.get());
    while (!queue.empty()) {
        RosterItem* item = *queue.begin();
        queue.pop_front();
        GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(item);
        ContactRosterItem *contact = dynamic_cast<ContactRosterItem*>(item);
        if (contact) {
            jids.insert(contact->getJID());
            jids.insert(contact->getDisplayJID());
        }
        else if (group) {
            queue.insert(queue.begin(), group->getChildren().begin(), group->getChildren().end());
        }
    }

    return jids;
}

GroupRosterItem* Roster::getGroup(const std::string& groupName) {
    for (auto* item : root_->getChildren()) {
        GroupRosterItem *group = dynamic_cast<GroupRosterItem*>(item);
        if (group && group->getDisplayName() == groupName) {
            return group;
        }
    }
    GroupRosterItem* group = new GroupRosterItem(groupName, root_.get(), sortByStatus_);
    root_->addChild(group);
    group->onChildrenChanged.connect(boost::bind(&Roster::handleChildrenChanged, this, group));
    group->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, group));
    return group;
}

void Roster::setBlockingSupported(bool isSupported) {
    if (!blockingSupported_) {
        for (auto i : itemMap_) {
            for (auto* item : i.second) {
                item->setBlockState(ContactRosterItem::IsUnblocked);
            }
        }
    }
    blockingSupported_ = isSupported;
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

void Roster::addContact(const JID& jid, const JID& displayJID, const std::string& name, const std::string& groupName, const boost::filesystem::path& avatarPath) {
    GroupRosterItem* group(getGroup(groupName));
    ContactRosterItem *item = new ContactRosterItem(jid, displayJID, name, group);
    item->onVCardRequested.connect(boost::bind(boost::ref(onVCardUpdateRequested), jid));
    item->setAvatarPath(avatarPath);
    if (blockingSupported_) {
        item->setBlockState(ContactRosterItem::IsUnblocked);
    }
    group->addChild(item);
    ItemMap::iterator i = itemMap_.insert(std::make_pair(fullJIDMapping_ ? jid : jid.toBare(), std::vector<ContactRosterItem*>())).first;
    if (!i->second.empty()) {
        for (const auto& existingGroup : i->second[0]->getGroups()) {
            item->addGroup(existingGroup);
        }
    }
    i->second.push_back(item);
    item->onDataChanged.connect(boost::bind(&Roster::handleDataChanged, this, item));
    filterContact(item, group);

    for (auto* item : i->second) {
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
    onChildrenChanged(root_.get());
    onDataChanged(root_.get());
}

void Roster::removeContact(const JID& jid) {
    ItemMap::iterator item = itemMap_.find(fullJIDMapping_ ? jid : jid.toBare());
    if (item != itemMap_.end()) {
        std::vector<ContactRosterItem*>& items = item->second;
        items.erase(std::remove_if(items.begin(), items.end(), JIDEqualsTo(jid)), items.end());
        if (items.empty()) {
            itemMap_.erase(item);
        }
    }
    //Causes the delete
    root_->removeChild(jid);
}

void Roster::removeContactFromGroup(const JID& jid, const std::string& groupName) {
    std::vector<RosterItem*> children = root_->getChildren();
    std::vector<RosterItem*>::iterator it = children.begin();
    ItemMap::iterator itemIt = itemMap_.find(fullJIDMapping_ ? jid : jid.toBare());
    while (it != children.end()) {
        GroupRosterItem* group = dynamic_cast<GroupRosterItem*>(*it);
        if (group && group->getDisplayName() == groupName) {
            auto deleted = group->removeChild(jid);
            if (itemIt != itemMap_.end()) {
                std::vector<ContactRosterItem*>& items = itemIt->second;
                items.erase(std::remove(items.begin(), items.end(), deleted.get()), items.end());
            }
        }
        ++it;
    }

    if (itemIt != itemMap_.end()) {
        for (auto* item : itemIt->second) {
            item->removeGroup(groupName);
        }
    }
}


void Roster::applyOnItems(const RosterItemOperation& operation) {
    if (operation.requiresLookup()) {
        applyOnItem(operation, operation.lookupJID());
    }
    else {
        applyOnAllItems(operation);
    }
}

void Roster::applyOnItem(const RosterItemOperation& operation, const JID& jid) {
    ItemMap::iterator i = itemMap_.find(fullJIDMapping_ ? jid : jid.toBare());
    if (i == itemMap_.end()) {
        return;
    }
    for (auto* item : i->second) {
        operation(item);
        filterContact(item, item->getParent());
    }
}

void Roster::applyOnAllItems(const RosterItemOperation& operation) {
    std::deque<RosterItem*> queue;
    queue.push_back(root_.get());
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

void Roster::addFilter(RosterFilter* filter) {
    filters_.push_back(filter);
    filterAll();
    onFilterAdded(filter);
}

void Roster::removeFilter(RosterFilter* filter) {
    filters_.erase(std::remove(filters_.begin(), filters_.end(), filter), filters_.end());
    filterAll();
    onFilterRemoved(filter);
}

void Roster::filterContact(ContactRosterItem* contact, GroupRosterItem* group) {
    size_t oldDisplayedSize = group->getDisplayedChildren().size();
    bool hide = true;
    for (auto* filter : filters_) {
        hide &= (*filter)(contact);
    }
    group->setDisplayed(contact, filters_.empty() || !hide);
    size_t newDisplayedSize = group->getDisplayedChildren().size();
    if (oldDisplayedSize == 0 && newDisplayedSize > 0) {
        onGroupAdded(group);
    }
}

void Roster::filterGroup(GroupRosterItem* group) {
    for (auto* child : group->getChildren()) {
        ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(child);
        if (contact) {
            filterContact(contact, group);
        }
    }
}

void Roster::filterAll() {
    std::deque<RosterItem*> queue;
    queue.push_back(root_.get());
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

