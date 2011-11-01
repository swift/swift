/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/Roster/QtTreeWidget.h"

#include "Swiften/Base/boost_bsignals.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"

namespace Swift {
class QtUIPreferences;

class QtOccupantListWidget : public QtTreeWidget {
	Q_OBJECT
	public:
		QtOccupantListWidget(UIEventStream* eventStream, QtUIPreferences* uiPreferences, QWidget* parent = 0);
		virtual ~QtOccupantListWidget();
		void setAvailableOccupantActions(const std::vector<ChatWindow::OccupantAction>& actions);
		boost::signal<void (ChatWindow::OccupantAction, ContactRosterItem*)> onOccupantActionSelected;
	protected:
		void contextMenuEvent(QContextMenuEvent* event);
	private:
		std::vector<ChatWindow::OccupantAction> availableOccupantActions_;
};

}

