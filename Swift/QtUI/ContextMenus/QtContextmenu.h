#pragma once

namespace Swift {
	class QtTreeWidgetItem;
	class QtContextMenu {
		public:
			virtual void show(QtTreeWidgetItem* item) = 0;
	};
}
