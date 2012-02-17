/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QTreeView>
#include <QModelIndex>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include "Swift/QtUI/Roster/RosterModel.h"
#include "Swift/QtUI/Roster/RosterDelegate.h"

namespace Swift {
class UIEventStream;
class SettingsProvider;

class QtTreeWidget : public QTreeView{
	Q_OBJECT
	public:
		QtTreeWidget(UIEventStream* eventStream, SettingsProvider* settings, QWidget* parent = 0);
		~QtTreeWidget();
		void show();
		QtTreeWidgetItem* getRoot();
		void setRosterModel(Roster* roster);
		Roster* getRoster() {return roster_;}
		boost::signal<void (RosterItem*)> onSomethingSelectedChanged;

	private slots:
		void handleItemActivated(const QModelIndex&);
		void handleModelItemExpanded(const QModelIndex&, bool expanded);
		void handleExpanded(const QModelIndex&);
		void handleCollapsed(const QModelIndex&);
		void handleClicked(const QModelIndex&);
		void handleSettingChanged(const std::string& setting);
	protected:
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);

	protected:
		QModelIndexList getSelectedIndexes() const;
	private:
		void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
	protected slots:
		virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);
	protected:
		UIEventStream* eventStream_;

	private:
		RosterModel* model_;
		Roster* roster_;
		RosterDelegate* delegate_;
		QtTreeWidgetItem* treeRoot_;
		SettingsProvider* settings_;
};

}
