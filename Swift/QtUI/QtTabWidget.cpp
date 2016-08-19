/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtTabWidget.h>

#include <Swift/QtUI/Trellis/QtDNDTabBar.h>

namespace Swift {

QtTabWidget::QtTabWidget(QWidget* parent) : QTabWidget(parent) {

}

QtTabWidget::~QtTabWidget() {

}

QTabBar* QtTabWidget::tabBar() {
    return QTabWidget::tabBar();
}

void QtTabWidget::setTabBar(QTabBar* tabBar) {
    QTabWidget::setTabBar(tabBar);
    if (dynamic_cast<QtDNDTabBar*>(tabBar)) {
        setAcceptDrops(true);
    }
    else {
        setAcceptDrops(false);
    }
}

void QtTabWidget::dragEnterEvent(QDragEnterEvent* event) {
    QtDNDTabBar* dndTabBar = dynamic_cast<QtDNDTabBar*>(tabBar());
    if (dndTabBar) {
        dndTabBar->dragEnterEvent(event);
    }
}

void QtTabWidget::dropEvent(QDropEvent* event) {
    QtDNDTabBar* dndTabBar = dynamic_cast<QtDNDTabBar*>(tabBar());
    if (dndTabBar) {
        dndTabBar->dropEvent(event);
    }
}

}
