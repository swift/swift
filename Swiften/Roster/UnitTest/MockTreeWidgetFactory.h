#ifndef SWIFTEN_MockTreeWidgetFactory_H
#define SWIFTEN_MockTreeWidgetFactory_H

#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/UnitTest/MockTreeWidget.h"
#include "Swiften/Roster/UnitTest/MockTreeWidgetItem.h"

namespace Swift {

class MockTreeWidgetItem;
class MockTreeWidget;

class MockTreeWidgetFactory : public TreeWidgetFactory {
	public:
		virtual ~MockTreeWidgetFactory() {}
		virtual TreeWidget* createTreeWidget() {
			return new MockTreeWidget();
		};
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidgetItem*) {
			return new MockTreeWidgetItem();
		};
		virtual TreeWidgetItem* createTreeWidgetItem(TreeWidget*) {
			return new MockTreeWidgetItem();
		}
};

}

#endif


