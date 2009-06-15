#pragma once

#include "QtTabbable.h"
#include <QWidget>
class QTabWidget;

namespace Swift {
	class QtChatTabs : public QWidget {
		Q_OBJECT
		public:
			QtChatTabs();
			void addTab(QtTabbable* tab);

		private slots:
			void handleTabClosing();
			void handleTabTitleUpdated();
			void handleTabCloseRequested(int index);
		private:
			QTabWidget* tabs_; 
	};
}

