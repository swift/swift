#pragma once

namespace Swift {
	class QtTreeWidgetItem;
	class QtContextMenu {
		public:
      virtual ~QtContextMenu();

			virtual void show(QtTreeWidgetItem* item) = 0;
	};
}
