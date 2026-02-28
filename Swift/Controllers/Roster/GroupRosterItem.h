/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swift/Controllers/Roster/ContactRosterItem.h>
#include <Swift/Controllers/Roster/RosterItem.h>

namespace Swift {

class GroupRosterItem : public RosterItem {
    public:
        GroupRosterItem(const std::string& name, GroupRosterItem* parent, bool sortByStatus);
        virtual ~GroupRosterItem();

        const std::vector<RosterItem*>& getChildren() const;
        const std::vector<RosterItem*>& getDisplayedChildren() const;

        void addChild(RosterItem* item);
        std::unique_ptr<ContactRosterItem> removeChild(const JID& jid);
        std::unique_ptr<GroupRosterItem> removeGroupChild(const std::string& group);
        void removeAll();

        void setDisplayed(RosterItem* item, bool displayed);
        void setExpanded(bool expanded);
        bool isExpanded() const;
        void setManualSort(const std::string& manualSortValue);
        virtual const std::string& getSortableDisplayName() const;

        boost::signals2::signal<void (bool)> onExpandedChanged;
        boost::signals2::signal<void ()> onChildrenChanged;

        static bool itemLessThanWithStatus(const RosterItem* left, const RosterItem* right);
        static bool itemLessThanWithoutStatus(const RosterItem* left, const RosterItem* right);

    private:
        void handleChildrenChanged(GroupRosterItem* group);
        void handleDataChanged(RosterItem* item);
        bool sortDisplayed();

    private:
        std::string name_;
        bool expanded_;
        std::vector<RosterItem*> children_;
        std::vector<RosterItem*> displayedChildren_;
        bool sortByStatus_;
        bool manualSort_;
        std::string manualSortValue_;
};

}

