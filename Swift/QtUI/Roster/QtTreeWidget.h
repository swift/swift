/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QTreeView>
#include <QModelIndex>
#include "Swift/QtUI/Roster/RosterModel.h"
#include "Swift/QtUI/Roster/RosterDelegate.h"

namespace Swift {
class UIEventStream;

class QtTreeWidget : public QTreeView{
	Q_OBJECT
	public:
		QtTreeWidget(UIEventStream* eventStream, QWidget* parent = 0);
		~QtTreeWidget();
		void show();
		QtTreeWidgetItem* getRoot();
		void setRosterModel(Roster* roster);
		Roster* getRoster() {return roster_;}
		void setEditable(bool b) { editable_ = b; }

	signals:
		void onSomethingSelectedChanged(bool);

	public slots:
		void handleEditUserActionTriggered(bool checked);

	private slots:
		void handleItemActivated(const QModelIndex&);
		void handleModelItemExpanded(const QModelIndex&, bool expanded);
		void handleExpanded(const QModelIndex&);
		void handleCollapsed(const QModelIndex&);
		void handleClicked(const QModelIndex&);
	protected:
		void contextMenuEvent(QContextMenuEvent* event);
	protected slots:
		virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);

	private:
		void renameGroup(GroupRosterItem* group);
		void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
		QModelIndexList getSelectedIndexes() const;
	
	private:
		RosterModel* model_;
		Roster* roster_;
		RosterDelegate* delegate_;
		QtTreeWidgetItem* treeRoot_;
		UIEventStream* eventStream_;
		bool editable_;
};

}
