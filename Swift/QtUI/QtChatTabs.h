#pragma once

#include "QtTabbable.h"
#include <QWidget>
#include <QRect>

class QTabWidget;

namespace Swift {
	class QtChatTabs : public QWidget {
		Q_OBJECT
		public:
			QtChatTabs();
			void addTab(QtTabbable* tab);
			void minimise();
		signals:
			void geometryChanged();

		protected slots:
			void closeEvent(QCloseEvent* event);
			void resizeEvent(QResizeEvent* event);
			void moveEvent(QMoveEvent* event);

		private slots:
			void handleTabClosing();
			void handleTabTitleUpdated();
			void handleTabTitleUpdated(QWidget* widget);
			void handleTabCloseRequested(int index);
			void handleWidgetShown();
			void handleWantsToActivate();
		private:
			QTabWidget* tabs_;
	};
}

