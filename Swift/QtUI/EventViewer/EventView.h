#pragma once

#include <QListView>

namespace Swift {
	class EventView : public QListView {
		Q_OBJECT
		public:
			EventView(QWidget* parent);
	};
}
