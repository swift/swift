#pragma once
#include <QTabWidget>

namespace Swift {
	class QtTabWidget : public QTabWidget {
		Q_OBJECT
		public:
			QtTabWidget(QWidget* parent);
			~QtTabWidget();
			QTabBar* tabBar();
	};
}
