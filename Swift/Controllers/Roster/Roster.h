/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
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
        boost::signal<void (GroupRosterItem*)> onChildrenChanged;
        boost::signal<void (GroupRosterItem*)> onGroupAdded;
        boost::signal<void (RosterItem*)> onDataChanged;
        boost::signal<void (JID&)> onVCardUpdateRequested;
        boost::signal<void (RosterFilter* filter)> onFilterAdded;
        boost::signal<void (RosterFilter* filter)> onFilterRemoved;
        GroupRosterItem* getGroup(const std::string& groupName);
        void setBlockingSupported(bool isSupported);

    private:
        void handleDataChanged(RosterItem* item);
        void handleChildrenChanged(GroupRosterItem* item);
        void filterGroup(GroupRosterItem* item);
        void filterContact(ContactRosterItem* contact, GroupRosterItem* group);
        void filterAll();
        GroupRosterItem* root_;
        std::vector<RosterFilter*> filters_;
        typedef std::map<JID, std::vector<ContactRosterItem*> > ItemMap;
        ItemMap itemMap_;
        bool fullJIDMapping_;
        bool sortByStatus_;
        bool blockingSupported_;
};

}
