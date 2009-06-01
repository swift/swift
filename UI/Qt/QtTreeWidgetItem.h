#ifndef SWIFT_QtTreeWidgetItem_H
#define SWIFT_QtTreeWidgetItem_H

#include <QColor>

#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "UI/Qt/QtTreeWidgetItem.h"
#include "UI/Qt/QtTreeWidget.h"
#include "UI/Qt/QtSwiftUtil.h"

namespace Swift {

class QtTreeWidgetItem : public QTreeWidgetItem, public TreeWidgetItem {
	public:
		QtTreeWidgetItem(QTreeWidget* parent) : QTreeWidgetItem(parent) {
		}

		QtTreeWidgetItem(QTreeWidgetItem* parent) : QTreeWidgetItem(parent) {
		}

		void setText(const String& text) {
			QTreeWidgetItem::setText(0, P2QSTRING(text));
		}

		void setTextColor(unsigned long color) {
			QTreeWidgetItem::setTextColor(0, QColor(
					((color & 0xFF0000)>>16),
					((color & 0xFF00)>>8), 
					(color & 0xFF)));
		}

		void setBackgroundColor(unsigned long color) {
			QTreeWidgetItem::setBackgroundColor(0, QColor(
					((color & 0xFF0000)>>16),
					((color & 0xFF00)>>8), 
					(color & 0xFF)));
		}

		void setExpanded(bool b) {
			treeWidget()->setItemExpanded(this, b);
		}

		void hide() {
			setHidden(true);
		}

		void show() {
			setHidden(false);
		}
};

}
#endif

