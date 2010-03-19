#pragma once

#include "boost/shared_ptr.hpp"

#include <QTreeView>

#include "Swift/Controllers/UIInterfaces/EventWindow.h"
#include "Swift/QtUI/EventViewer/EventView.h"
#include "Swift/QtUI/EventViewer/EventModel.h"
#include "Swift/QtUI/EventViewer/EventDelegate.h"

namespace Swift {
	class QtEventWindow : public QTreeView, public EventWindow {
		Q_OBJECT
		public:
			QtEventWindow(QWidget* parent = 0);
			~QtEventWindow();
			void addEvent(boost::shared_ptr<Event> event, bool active);
			void removeEvent(boost::shared_ptr<Event> event);
		private:
			EventModel* model_;
			EventDelegate* delegate_;
	};
		
}

