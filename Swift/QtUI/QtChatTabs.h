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
			void tabClosing();
			void tabTitleUpdated();
		private:
			QTabWidget* tabs_; 
	};
}

