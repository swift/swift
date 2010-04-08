/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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
