/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtTreeWidget_H
#define SWIFT_QtTreeWidget_H

#include <QTreeView>
#include <QModelIndex>
#include "Swift/QtUI/Roster/QtTreeWidget.h"
#include "Swift/QtUI/Roster/RosterModel.h"
#include "Swift/QtUI/Roster/RosterDelegate.h"
#include "Swift/QtUI/ContextMenus/QtContextMenu.h"

namespace Swift {
class UIEventStream;

class QtTreeWidget : public QTreeView{
	Q_OBJECT
	public:
		QtTreeWidget(UIEventStream* eventStream, QWidget* parent = 0);
		~QtTreeWidget();
		void show();
		QtTreeWidgetItem* getRoot();
		void setContextMenu(QtContextMenu* contextMenu);
		void setRosterModel(Roster* roster);
		Roster* getRoster() {return roster_;}
	private slots:
		void handleItemActivated(const QModelIndex&);
		void handleModelItemExpanded(const QModelIndex&, bool expanded);
		void handleExpanded(const QModelIndex&);
		void handleCollapsed(const QModelIndex&);
		void handleClicked(const QModelIndex&);
	protected:
		void contextMenuEvent(QContextMenuEvent* event);

	private:
		void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
		RosterModel* model_;
		Roster* roster_;
		RosterDelegate* delegate_;
		QtTreeWidgetItem* treeRoot_;
		QtContextMenu* contextMenu_;
		UIEventStream* eventStream_;
};

}
#endif

