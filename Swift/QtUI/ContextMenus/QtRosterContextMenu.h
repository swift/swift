/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>

#include "Swift/QtUI/ContextMenus/QtContextMenu.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"

namespace Swift {
	class RosterItem;
	class QtRosterContextMenu : public QObject, public QtContextMenu {
		Q_OBJECT
		public:
			QtRosterContextMenu(UIEventStream* eventStream);
			void show(RosterItem* item);

		private slots:
			void handleRemove();

		private:
			RosterItem* item_;
			UIEventStream* eventStream_;
	};
}
