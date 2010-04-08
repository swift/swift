/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtTabWidget.h"

namespace Swift {

QtTabWidget::QtTabWidget(QWidget* parent) : QTabWidget(parent) {

}

QtTabWidget::~QtTabWidget() {

}

QTabBar* QtTabWidget::tabBar() {
	return QTabWidget::tabBar();
}

}
