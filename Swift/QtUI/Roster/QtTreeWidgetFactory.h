#ifndef SWIFT_QtTreeWidgetFactory_H
#define SWIFT_QtTreeWidgetFactory_H

#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"

namespace Swift {

class QtTreeWidgetFactory : public TreeWidgetFactory {
	public:
		QtTreeWidgetFactory() {
		}

		TreeWidget* createTreeWidget() {
			return new QtTreeWidget();
		}

		TreeWidgetItem* createTreeWidgetItem(TreeWidgetItem* item) {
			QtTreeWidgetItem* qtItem = dynamic_cast<QtTreeWidgetItem*>(item);
			assert(qtItem);
			QtTreeWidgetItem* newItem = new QtTreeWidgetItem(qtItem);
			qtItem->addChild(newItem);
			return newItem;
		}

		TreeWidgetItem* createTreeWidgetItem(TreeWidget* item) {
			QtTreeWidget* qtItem = dynamic_cast<QtTreeWidget*>(item);
			assert(qtItem);
			QtTreeWidgetItem* newItem = new QtTreeWidgetItem(qtItem);
			//qtItem->setItemWidget(newItem, 0, newItem->getCollapsedRosterWidget());
			qtItem->addChild(newItem);
			return newItem;
		}
};

}
#endif

