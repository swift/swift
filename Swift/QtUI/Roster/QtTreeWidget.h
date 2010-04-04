#ifndef SWIFT_QtTreeWidget_H
#define SWIFT_QtTreeWidget_H

#include <QTreeView>
#include <QModelIndex>
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidget.h"
#include "Swift/QtUI/Roster/RosterModel.h"
#include "Swift/QtUI/Roster/RosterDelegate.h"
#include "Swift/QtUI/ContextMenus/QtContextMenu.h"

namespace Swift {

class QtTreeWidget : public QTreeView, public TreeWidget {
	Q_OBJECT
	public:
		QtTreeWidget(QWidget* parent = 0);
		~QtTreeWidget();
		void show();
		QtTreeWidgetItem* getRoot();
		void setContextMenu(QtContextMenu* contextMenu);
	private slots:
		void handleItemActivated(const QModelIndex&);
		void handleModelItemExpanded(const QModelIndex&, bool expanded);
		void handleExpanded(const QModelIndex&);
		void handleCollapsed(const QModelIndex&);
		void handleDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	protected:
		void contextMenuEvent(QContextMenuEvent* event);

	private:
		void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
		RosterModel* model_;
		RosterDelegate* delegate_;
		QtTreeWidgetItem* treeRoot_;
		QtContextMenu* contextMenu_;
};

}
#endif

