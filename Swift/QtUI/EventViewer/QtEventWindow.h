/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QTreeView>

#include "Swift/Controllers/UIInterfaces/EventWindow.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/QtUI/EventViewer/EventView.h"
#include "Swift/QtUI/EventViewer/EventModel.h"
#include "Swift/QtUI/EventViewer/EventDelegate.h"

class QPushButton;

namespace Swift {
	class QtEventWindow : public QWidget, public EventWindow {
		Q_OBJECT
		public:
			QtEventWindow(UIEventStream* eventStream);
			~QtEventWindow();
			void addEvent(boost::shared_ptr<StanzaEvent> event, bool active);
			void removeEvent(boost::shared_ptr<StanzaEvent> event);
		signals:
			void onNewEventCountUpdated(int count);
		private slots:
			void handleItemActivated(const QModelIndex& item);
			void handleItemClicked(const QModelIndex& item);
			void handleReadClicked();
		private:
			EventModel* model_;
			EventDelegate* delegate_;
			UIEventStream* eventStream_;
			QTreeView* view_;
			QPushButton* readButton_;
	};
		
}

