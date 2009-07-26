#ifndef SWIFT_QtTreeWidgetItem_H
#define SWIFT_QtTreeWidgetItem_H

#include <QColor>

#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/RosterItem.h"


#include "Swift/QtUI/QtSwiftUtil.h"


namespace Swift {
class QtTreeWidget;
class QtTreeWidgetItem : public TreeWidgetItem, public RosterItem {
	public:
		QtTreeWidgetItem(RosterItem* parentItem) : RosterItem(parentItem) {

		}

		void setText(const String& text) {
			displayName_ = P2QSTRING(text);
			// QTreeWidgetItem::setText(0, P2QSTRING(text));
		}

		void setTextColor(unsigned long color) {
			// QTreeWidgetItem::setTextColor(0, QColor(
			// 					((color & 0xFF0000)>>16),
			// 					((color & 0xFF00)>>8), 
			// 					(color & 0xFF)));
		}

		void setBackgroundColor(unsigned long color) {
			// QTreeWidgetItem::setBackgroundColor(0, QColor(
			// 					((color & 0xFF0000)>>16),
			// 					((color & 0xFF00)>>8), 
			// 					(color & 0xFF)));
		}

		void setExpanded(bool b) {
			//treeWidget()->setItemExpanded(this, b);
		}

		void hide() {
			//setHidden(true);
		}

		void show() {
			//setHidden(false);
		}
		
		QWidget* getCollapsedRosterWidget();
		QWidget* getExpandedRosterWidget();
		
	private:
		QString displayName_;
};

}
#endif

