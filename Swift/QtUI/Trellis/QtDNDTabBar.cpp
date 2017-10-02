/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/Trellis/QtDNDTabBar.h>

#include <cassert>

#include <QDrag>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QTabWidget>

namespace Swift {

QtDNDTabBar::QtDNDTabBar(QWidget* parent) : QTabBar(parent) {
    setAcceptDrops(true);

    // detect the default tab bar height;
    insertTab(0, "");
    defaultTabHeight = QTabBar::sizeHint().height();
    removeTab(0);
}

QtDNDTabBar::~QtDNDTabBar() {

}

int QtDNDTabBar::getDragIndex() const {
    return dragIndex;
}

QString QtDNDTabBar::getDragText() const {
    return dragText;
}

QWidget* QtDNDTabBar::getDragWidget() const {
    return dragWidget;
}

QSize QtDNDTabBar::sizeHint() const {
    QSize hint = QTabBar::sizeHint();
    if (hint.isEmpty()) {
        hint = QSize(parentWidget()->width(), defaultTabHeight);
    }
    return hint;
}

QSize QtDNDTabBar::tabSizeHint(int index) const {
    QSize tabSize = QTabBar::tabSizeHint(index);
#if defined(Q_OS_MAC)
    // With multiple tabs having the same label in a QTabBar, the size hint computed by
    // Qt on OS X is too small and it is elided even though there is enough horizontal
    // space available. We work around this issue by adding the width of a letter to the
    // size hint.
    tabSize += QSize(QFontMetrics(font()).width("I"), 0);
#endif
    return tabSize;
}

void QtDNDTabBar::dragEnterEvent(QDragEnterEvent* dragEnterEvent) {
    QtDNDTabBar* sourceTabBar = dynamic_cast<QtDNDTabBar*>(dragEnterEvent->source());
    if (sourceTabBar) {
        dragEnterEvent->acceptProposedAction();
    }
}

void QtDNDTabBar::dropEvent(QDropEvent* dropEvent) {
    auto sourceTabBar = dynamic_cast<QtDNDTabBar*>(dropEvent->source());
    if (sourceTabBar && dropEvent->mimeData() && dropEvent->mimeData()->data("action") == QByteArray("application/tab-detach")) {
        int targetTabIndex = tabAt(dropEvent->pos());
        QRect rect = tabRect(targetTabIndex);
        if (targetTabIndex >= 0 && (dropEvent->pos().x() - rect.left() - rect.width()/2 > 0)) {
            targetTabIndex++;
        }

        QWidget* tab = sourceTabBar->getDragWidget();
        assert(tab);
        QTabWidget* targetTabWidget = dynamic_cast<QTabWidget*>(parentWidget());

        QString tabText = sourceTabBar->getDragText();

        /*
         * When you add a widget to an empty QTabWidget, it's automatically made the current widget.
         * Making the widget the current widget, widget->show() is called for the widget. Directly reacting
         * to that event, and adding the widget again to the QTabWidget results in undefined behavior. For
         * example the tab label is shown but the widget is neither has the old nor in the new QTabWidget as
         * parent. Blocking signals on the QWidget to be added to a QTabWidget prevents this behavior.
         */
        targetTabWidget->setUpdatesEnabled(false);
        tab->blockSignals(true);
        targetTabWidget->insertTab(targetTabIndex, tab, tabText);
        dropEvent->acceptProposedAction();
        tab->blockSignals(false);
        targetTabWidget->setUpdatesEnabled(true);
        onDropSucceeded();
    }
}

bool QtDNDTabBar::event(QEvent* event) {
    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (mouseEvent) {
        QWidget* childAtPoint = window()->childAt(mapTo(window(), mouseEvent->pos()));
        QtDNDTabBar* underMouse = dynamic_cast<QtDNDTabBar*>(childAtPoint);
        if (!underMouse && childAtPoint) {
            underMouse = dynamic_cast<QtDNDTabBar*>(childAtPoint->parent());
        }
        if (!underMouse && currentIndex() >= 0) {
            // detach and drag

            // stop move event
            QMouseEvent* finishMoveEvent = new QMouseEvent (QEvent::MouseMove, mouseEvent->pos (), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            QTabBar::event(finishMoveEvent);
            delete finishMoveEvent;
            finishMoveEvent = nullptr;

            // start drag
            QDrag* drag = new QDrag(this);
            QMimeData* mimeData = new QMimeData;

            // distinguish tab-reordering drops from other ones
            mimeData->setData("action", "application/tab-detach") ;
            drag->setMimeData(mimeData);

            // set drag image
            QRect rect = tabRect( currentIndex() );
#if QT_VERSION >= 0x050000
            QPixmap pixmap = grab(rect);
#else
            QPixmap pixmap = QPixmap::grabWidget(this, rect);
#endif
            QPixmap targetPixmap (pixmap.size ());
            QPainter painter (&targetPixmap);
            painter.setOpacity(0.9);
            painter.drawPixmap(0,0, pixmap);
            painter.end ();
            drag->setPixmap (targetPixmap);

            drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()));

            dragIndex = currentIndex();
            dragText = tabText(dragIndex);
            dragWidget = dynamic_cast<QTabWidget*>(parent())->widget(dragIndex);
            assert(dragWidget);
            dynamic_cast<QTabWidget*>(parent())->removeTab(currentIndex());
            Qt::DropAction dropAction = drag->exec();
            if (dropAction == Qt::IgnoreAction) {
                // aborted drag, put tab back in place
                // disable event handling during the insert for the tab to prevent infinite recursion (stack overflow)
                dragWidget->blockSignals(true);
                dynamic_cast<QTabWidget*>(parent())->insertTab(dragIndex, dragWidget, dragText);
                dragWidget->blockSignals(false);
            }
            return true;
        }
    }
    return QTabBar::event(event);
}

}
