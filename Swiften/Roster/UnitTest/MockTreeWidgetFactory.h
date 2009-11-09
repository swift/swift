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
			MockTreeWidgetItem* entry = new MockTreeWidgetItem();
			groupMembers_[group].push_back(entry);
			return entry;
		};
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidget*) {
			MockTreeWidgetItem* group = new MockTreeWidgetItem();
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
	private:
		std::vector<MockTreeWidgetItem*> groups_;
		std::map<TreeWidgetItem*, std::vector<MockTreeWidgetItem*> > groupMembers_;
		MockTreeWidget* root_;
};

}

#endif


