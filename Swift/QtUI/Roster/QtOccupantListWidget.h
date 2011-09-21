/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/Roster/QtTreeWidget.h"

namespace Swift {

class QtOccupantListWidget : public QtTreeWidget {
	Q_OBJECT
	public:
		QtOccupantListWidget(UIEventStream* eventStream, QWidget* parent = 0);
		virtual ~QtOccupantListWidget();
	protected:
		void contextMenuEvent(QContextMenuEvent* event);
};

}

