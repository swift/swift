/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include "Swiften/JID/JID.h"
#include "Swift/Controllers/Roster/RosterItemOperation.h"
#include "Swift/Controllers/Roster/RosterFilter.h"
#include <Swift/Controllers/Roster/ContactRosterItem.h>

#include <vector>
#include <map>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

namespace Swift {

class RosterItem;
class GroupRosterItem;
class ContactRosterItem;

class Roster {
	public:
		Roster(bool sortByStatus = true, bool fullJIDMapping = false);
		~Roster();

		void addContact(const JID& jid, const JID& displayJID, const std::string& name, const std::string& group, const std::string& avatarPath);
		void removeContact(const JID& jid);
		void removeContactFromGroup(const JID& jid, const std::string& group);
		void removeGroup(const std::string& group);
		void removeAll();
		void applyOnItems(const RosterItemOperation& operation);
		void applyOnAllItems(const RosterItemOperation& operation);
		void applyOnItem(const RosterItemOperation& operation, const JID& jid);
		void addFilter(RosterFilter *filter) {filters_.push_back(filter);filterAll();};
		void removeFilter(RosterFilter *filter);
		GroupRosterItem* getRoot();
		std::vector<RosterFilter*> getFilters() {return filters_;};
		boost::signal<void (GroupRosterItem*)> onChildrenChanged;
		boost::signal<void (GroupRosterItem*)> onGroupAdded;
		boost::signal<void (RosterItem*)> onDataChanged;
		GroupRosterItem* getGroup(const std::string& groupName);
		void setAvailableFeatures(const JID& jid, const std::set<ContactRosterItem::Feature>& features);

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
};

}
