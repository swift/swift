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
		public:
			QtSingleWindow(QtSettingsProvider* settings);
			virtual ~QtSingleWindow();
		protected:
			void resizeEvent(QResizeEvent*);
			void moveEvent(QMoveEvent*);
		private:
			void handleGeometryChanged();

		private:

			QtSettingsProvider* settings_;
	};

}

