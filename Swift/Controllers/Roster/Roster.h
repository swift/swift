/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/ItemOperations/RosterItemOperation.h>
#include <Swift/Controllers/Roster/RosterFilter.h>

namespace Swift {

class RosterItem;
class GroupRosterItem;
class ContactRosterItem;

class Roster {
    public:
        Roster(bool sortByStatus = true, bool fullJIDMapping = false);
        ~Roster();

        void addContact(const JID& jid, const JID& displayJID, const std::string& name, const std::string& group, const boost::filesystem::path& avatarPath);
        void removeContact(const JID& jid);
        void removeContactFromGroup(const JID& jid, const std::string& group);
        void removeGroup(const std::string& group);
        void removeAll();
        void applyOnItems(const RosterItemOperation& operation);
        void applyOnAllItems(const RosterItemOperation& operation);
        void applyOnItem(const RosterItemOperation& operation, const JID& jid);
        void addFilter(RosterFilter* filter);
        void removeFilter(RosterFilter* filter);
        GroupRosterItem* getRoot() const;
        std::set<JID> getJIDs() const;

        std::vector<RosterFilter*> getFilters() {return filters_;}
        boost::signals2::signal<void (GroupRosterItem*)> onChildrenChanged;
        boost::signals2::signal<void (GroupRosterItem*)> onGroupAdded;
        boost::signals2::signal<void (RosterItem*)> onDataChanged;
        boost::signals2::signal<void (JID&)> onVCardUpdateRequested;
        boost::signals2::signal<void (RosterFilter* filter)> onFilterAdded;
        boost::signals2::signal<void (RosterFilter* filter)> onFilterRemoved;
        GroupRosterItem* getGroup(const std::string& groupName);
        void setBlockingSupported(bool isSupported);

    private:
        void handleDataChanged(RosterItem* item);
        void handleChildrenChanged(GroupRosterItem* item);
        void filterGroup(GroupRosterItem* item);
        void filterContact(ContactRosterItem* contact, GroupRosterItem* group);
        void filterAll();

    private:
        std::vector<RosterFilter*> filters_;
        typedef std::map<JID, std::vector<ContactRosterItem*> > ItemMap;
        ItemMap itemMap_;
        bool fullJIDMapping_;
        bool sortByStatus_;
        bool blockingSupported_ = false;
        const std::unique_ptr<GroupRosterItem> root_;
};

}
