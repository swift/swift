/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QTabWidget>

namespace Swift {
	class QtTabWidget : public QTabWidget {
	Q_OBJECT
		public:
			QtTabWidget(QWidget* parent);
			virtual ~QtTabWidget();

			QTabBar* tabBar();
			void setTabBar(QTabBar* tabBar);
	};
}
