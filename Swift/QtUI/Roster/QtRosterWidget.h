/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/Roster/QtTreeWidget.h"

namespace Swift {

class QtRosterWidget : public QtTreeWidget {
	Q_OBJECT
	public:
		QtRosterWidget(UIEventStream* eventStream, QWidget* parent = 0);
		virtual ~QtRosterWidget();
	public slots:
		void handleEditUserActionTriggered(bool checked);
	signals:
		void onSomethingSelectedChanged(bool);
	protected:
		void contextMenuEvent(QContextMenuEvent* event);
	protected slots:
		virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous);
	private:
		void renameGroup(GroupRosterItem* group);
};

}
