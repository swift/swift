#pragma once

#include <QObject>

#include "Swift/QtUI/ContextMenus/QtContextMenu.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"

namespace Swift {
	class QtRosterContextMenu : public QObject, public QtContextMenu {
		Q_OBJECT
		public:
			QtRosterContextMenu(UIEventStream* eventStream);
			void show(QtTreeWidgetItem* item);

		private slots:
			void handleRemove();

		private:
			QtTreeWidgetItem* item_;
			UIEventStream* eventStream_;
	};
}
