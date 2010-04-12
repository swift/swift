/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MockTreeWidgetFactory_H
#define SWIFTEN_MockTreeWidgetFactory_H

#include "Swiften/Roster/TreeWidgetFactory.h"

#include <vector>
#include "Swiften/Base/foreach.h"
#include "Swiften/Roster/UnitTest/MockTreeWidget.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetItem.h"

namespace Swift {

class MockTreeWidgetItem;
class MockTreeWidget;

class MockTreeWidgetFactory : public TreeWidgetFactory {
	public:
		virtual ~MockTreeWidgetFactory() {}
		virtual TreeWidget* createTreeWidget() {
			root_ = new MockTreeWidget();
			return root_;
		};
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidgetItem* group) {
			MockTreeWidgetItem* entry = new MockTreeWidgetItem(this);
			groupMembers_[group].push_back(entry);
			return entry;
		};
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidget*) {
			MockTreeWidgetItem* group = new MockTreeWidgetItem(this);
			groups_.push_back(group);
			return group;
		};
		virtual std::vector<String> getGroups() {
			std::vector<String> groupNames;
			foreach (MockTreeWidgetItem* group, groups_) {
				groupNames.push_back(group->getText());
			}
			return groupNames;
		};

		typedef std::map<TreeWidgetItem*, std::vector<MockTreeWidgetItem*> > itemMap;

		virtual std::vector<MockTreeWidgetItem*> getGroupMembers(const String& group) {
			for (itemMap::iterator it = groupMembers_.begin(); it != groupMembers_.end(); it++) {
				if (((MockTreeWidgetItem*)(it->first))->getText() == group) {
					return it->second;
				}
			}
			return std::vector<MockTreeWidgetItem*>();
		};

		virtual void removeItem(MockTreeWidgetItem* item) {
			foreach (TreeWidgetItem* groupItem, groups_) {
				std::vector<MockTreeWidgetItem*>& members = groupMembers_[groupItem];
				members.erase(std::remove(members.begin(), members.end(), item), members.end());
			}
		};
	private:
		std::vector<MockTreeWidgetItem*> groups_;
		std::map<TreeWidgetItem*, std::vector<MockTreeWidgetItem*> > groupMembers_;
		MockTreeWidget* root_;
};

}

#endif


