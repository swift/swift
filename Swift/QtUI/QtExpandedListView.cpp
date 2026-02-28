/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtExpandedListView.h>

#include <QWheelEvent>
#include <QScrollArea>
#include <QDebug>

namespace Swift {

QtExpandedListView::QtExpandedListView(QWidget* parent) : QListView(parent) {
    // Disable macOS focus rectangle due to bad performance.
    setAttribute(Qt::WA_MacShowFocusRect, 0);
    setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

void QtExpandedListView::setModel(QAbstractItemModel* newModel) {
    if (model()) {
        disconnectFromModel(model());
    }
    if (newModel) {
        connectToModel(newModel);
    }
    QListView::setModel(newModel);
    adjustHeightToModelChange();
}

QtExpandedListView::~QtExpandedListView() {
    if (model()) {
        disconnectFromModel(model());
    }
}

bool QtExpandedListView::viewportEvent(QEvent* event) {
    // Ignore wheel events for faster mouse scrolling.
    if (event && event->type() == QEvent::Wheel) {
        return false;
    }

    return QListView::viewportEvent(event);
}

template <typename T>
T getParentOfType(QWidget* start) {
    auto parentW = start->parentWidget();
    if (parentW == nullptr) {
        return nullptr;
    }
    T result = dynamic_cast<T>(parentW);
    if (result) {
        return result;
    }
    return getParentOfType<T>(parentW);
}

void QtExpandedListView::currentChanged(const QModelIndex &current, const QModelIndex &) {
    // Make sure that the current selected index is visible in the parent QScrollArea.
    auto scrollArea = getParentOfType<QScrollArea*>(parentWidget());
    if (scrollArea) {
        auto scrollWidget = scrollArea->widget();
        QList<QPoint> points;
        auto visRect = visualRect(current);
        points << mapTo(scrollWidget, visRect.topLeft());
        points << mapTo(scrollWidget, visRect.topRight());
        points << mapTo(scrollWidget, visRect.bottomLeft());
        points << mapTo(scrollWidget, visRect.bottomRight());

        for (auto&& point : points) {
            scrollArea->ensureVisible(point.x(), point.y(), 0, 0);
        }
    }
}

void QtExpandedListView::adjustHeightToModelChange() {
    updateGeometry();
}

QSize QtExpandedListView::minimumSizeHint() const {
    auto sh = sizeHint();
    return QSize(0, sh.height());
}

QSize QtExpandedListView::sizeHint() const {
    auto listViewSH = QListView::sizeHint();
    if (model()) {
        auto lastRect = rectForIndex(model()->index(model()->rowCount()-1, 0, rootIndex()));
        auto idealHeight = lastRect.y() + lastRect.height() + frameWidth() * 2;
        listViewSH.setHeight(idealHeight);
    }
    return listViewSH;
}

void QtExpandedListView::connectToModel(QAbstractItemModel* model) {
    connect(model, &QAbstractItemModel::dataChanged, this, &QtExpandedListView::adjustHeightToModelChange);
    connect(model, &QAbstractItemModel::modelReset, this, &QtExpandedListView::adjustHeightToModelChange);
    connect(model, &QAbstractItemModel::rowsInserted, this, &QtExpandedListView::adjustHeightToModelChange);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &QtExpandedListView::adjustHeightToModelChange);
}

void QtExpandedListView::disconnectFromModel(QAbstractItemModel* model) {
    disconnect(model, &QAbstractItemModel::dataChanged, this, &QtExpandedListView::adjustHeightToModelChange);
    disconnect(model, &QAbstractItemModel::modelReset, this, &QtExpandedListView::adjustHeightToModelChange);
    disconnect(model, &QAbstractItemModel::rowsInserted, this, &QtExpandedListView::adjustHeightToModelChange);
    disconnect(model, &QAbstractItemModel::rowsRemoved, this, &QtExpandedListView::adjustHeightToModelChange);
}

}
