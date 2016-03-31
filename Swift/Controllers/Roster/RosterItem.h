/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
class GroupRosterItem;
class RosterItem {
    public:
        RosterItem(const std::string& name, GroupRosterItem* parent);
        virtual ~RosterItem();
        boost::signal<void ()> onDataChanged;
        GroupRosterItem* getParent() const;
        void setDisplayName(const std::string& name);
        const std::string& getDisplayName() const;
        virtual const std::string& getSortableDisplayName() const;
    private:
        std::string name_;
        std::string sortableDisplayName_;
        GroupRosterItem* parent_;
};

}

