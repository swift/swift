#ifndef SWIFT_QtTreeWidget_H
#define SWIFT_QtTreeWidget_H

#include <QTreeView>

#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"
#include "Swift/QtUI/Roster/RosterModel.h"
#include "Swift/QtUI/Roster/RosterItem.h"

namespace Swift {

class QtTreeWidget : public QTreeView, public TreeWidget, public RosterItem {
	Q_OBJECT
	public:
		QtTreeWidget(QWidget* parent = 0);
		void show();
	//private slots:
	//	void handleItemActivated(QTreeWidgetItem*, int);

	private:
		void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
		RosterModel* model_;
		
};

}
#endif

