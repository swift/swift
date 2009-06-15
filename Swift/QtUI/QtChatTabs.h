#pragma once

#include <QWidget>

class QTabWidget;

namespace Swift {
	class QtChatTabs : public QWidget {
		Q_OBJECT
		public:
			QtChatTabs();
			void addTab(QWidget* tab);

		private slots:
			void tabClosing();

		private:
			QTabWidget* tabs_; 
	};
}

