/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "QtTabbable.h"
#include "QtTabWidget.h"
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
			QtTabbable* getCurrentTab();
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
			void handleRequestedPreviousTab();
			void handleRequestedNextTab();
			void handleRequestedActiveTab();
			void flash();

		private:
			void checkForFirstShow();
			QtTabWidget* tabs_;
	};
}

