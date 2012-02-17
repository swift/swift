/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/Roster/QtTreeWidget.h"

namespace Swift {
class QtUIPreferences;

class QtRosterWidget : public QtTreeWidget {
	Q_OBJECT
	public:
		QtRosterWidget(UIEventStream* eventStream, SettingsProvider* settings, QWidget* parent = 0);
		virtual ~QtRosterWidget();
	public slots:
		void handleEditUserActionTriggered(bool checked);
	protected:
		void contextMenuEvent(QContextMenuEvent* event);
	private:
		void renameGroup(GroupRosterItem* group);
};

}
