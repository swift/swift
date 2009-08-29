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
			void minimise();

		protected slots:
			void closeEvent(QCloseEvent* event);

		private slots:
			void handleTabClosing();
			void handleTabTitleUpdated();
			void handleTabCloseRequested(int index);
			void handleWidgetShown();
			void handleWantsToActivate();
		private:
			QTabWidget* tabs_;
	};
}

