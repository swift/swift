#ifndef SWIFT_QtTreeWidgetFactory_H
#define SWIFT_QtTreeWidgetFactory_H

#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/QtTreeWidgetItem.h"
#include "Swift/QtUI/QtTreeWidget.h"

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
			return new QtTreeWidgetItem(qtItem);
		}

		TreeWidgetItem* createTreeWidgetItem(TreeWidget* item) {
			QtTreeWidget* qtItem = dynamic_cast<QtTreeWidget*>(item);
			assert(qtItem);
			return new QtTreeWidgetItem(qtItem);
		}
};

}
#endif

