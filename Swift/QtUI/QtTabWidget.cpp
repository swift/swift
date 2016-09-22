/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtTabWidget.h>

#include <QLabel>
#include <QPainter>
#include <QPoint>

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

void QtTabWidget::paintEvent(QPaintEvent * event) {
    QTabWidget::paintEvent(event);
    if (count() == 0) {
        QLabel label;
        label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        label.setGeometry(QRect(QPoint(0,0), size()) - QMargins(10,10,10,10));
        label.setWordWrap(true);
        label.setText(tr("This empty cell is a placeholder for chat windows. You can move existing chats to this cell by dragging the tab over here. You can change the number of cells via the 'Change layout' dialog under the 'View' menu or by using the %1 shortcut.").arg(QKeySequence(tr("Ctrl+Alt+L")).toString(QKeySequence::NativeText)));
        QPainter painter(this);
        painter.drawPixmap(label.geometry().topLeft(), label.grab());
    }
}

}
