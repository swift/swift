/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QSplitter>

namespace Swift {
	class QtSettingsProvider;

	class QtSingleWindow : public QSplitter {
		Q_OBJECT
		public:
			QtSingleWindow(QtSettingsProvider* settings);
			virtual ~QtSingleWindow();
			void insertAtFront(QWidget* widget);
		protected:
			void resizeEvent(QResizeEvent*);
			void moveEvent(QMoveEvent*);
		private slots:
			void handleSplitterMoved(int, int);
		private:
			void handleGeometryChanged();
			void restoreSplitters();

		private:

			QtSettingsProvider* settings_;
	};

}

