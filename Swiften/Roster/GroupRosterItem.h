/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Roster/RosterItem.h"
#include "Swiften/Base/String.h"
#include "Swiften/Roster/ContactRosterItem.h"

#include <vector>

namespace Swift {

class GroupRosterItem : public RosterItem {
	public:
		GroupRosterItem(const String& name, GroupRosterItem* parent);
		virtual ~GroupRosterItem();
		const std::vector<RosterItem*>& getChildren() const;
		const std::vector<RosterItem*>& getDisplayedChildren() const;
		void addChild(RosterItem* item);
		ContactRosterItem* removeChild(const JID& jid);
		void setDisplayed(RosterItem* item, bool displayed);
		boost::signal<void ()> onChildrenChanged;
		static bool itemLessThan(const RosterItem* left, const RosterItem* right);
		void setExpanded(bool expanded);
		bool isExpanded() const;
	private:
		void handleChildrenChanged(GroupRosterItem* group);
		void handleDataChanged(RosterItem* item);
		bool sortDisplayed();
		String name_;
		bool expanded_;
		std::vector<RosterItem*> children_;
		std::vector<RosterItem*> displayedChildren_;
};

}

