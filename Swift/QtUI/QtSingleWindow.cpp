/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSingleWindow.h>

#include <Swift/QtUI/QtChatTabs.h>
#include <Swift/QtUI/QtSettingsProvider.h>

namespace Swift {

static const QString SINGLE_WINDOW_GEOMETRY = QString("SINGLE_WINDOW_GEOMETRY");
static const QString SINGLE_WINDOW_SPLITS = QString("SINGLE_WINDOW_SPLITS");

QtSingleWindow::QtSingleWindow(QtSettingsProvider* settings) : QSplitter() {
    settings_ = settings;
    QVariant geometryVariant = settings_->getQSettings()->value(SINGLE_WINDOW_GEOMETRY);
    if (geometryVariant.isValid()) {
        restoreGeometry(geometryVariant.toByteArray());
    }
    connect(this, SIGNAL(splitterMoved(int, int)), this, SLOT(handleSplitterMoved(int, int)));
    restoreSplitters();
}

QtSingleWindow::~QtSingleWindow() {

}

void QtSingleWindow::addWidget(QWidget* widget) {
    QtChatTabs* tabs = dynamic_cast<QtChatTabs*>(widget);
    if (tabs) {
        connect(tabs, SIGNAL(onTitleChanged(const QString&)), this, SLOT(handleTabsTitleChanged(const QString&)));
    }
    QSplitter::addWidget(widget);
}

void QtSingleWindow::handleTabsTitleChanged(const QString& title) {
    setWindowTitle(title);
}

void QtSingleWindow::handleSplitterMoved(int, int) {
    QList<QVariant> variantValues;
    QList<int> intValues = sizes();
    for (const auto& value : intValues) {
        variantValues.append(QVariant(value));
    }
    settings_->getQSettings()->setValue(SINGLE_WINDOW_SPLITS, QVariant(variantValues));
}

void QtSingleWindow::restoreSplitters() {
    QList<QVariant> variantValues = settings_->getQSettings()->value(SINGLE_WINDOW_SPLITS).toList();
    QList<int> intValues;
    for (auto&& value : variantValues) {
        intValues.append(value.toInt());
    }
    setSizes(intValues);
}

void QtSingleWindow::insertAtFront(QWidget* widget) {
    insertWidget(0, widget);
    restoreSplitters();
}

void QtSingleWindow::handleGeometryChanged() {
    settings_->getQSettings()->setValue(SINGLE_WINDOW_GEOMETRY, saveGeometry());
}

void QtSingleWindow::resizeEvent(QResizeEvent* event) {
    handleGeometryChanged();
    QSplitter::resizeEvent(event);
}

void QtSingleWindow::moveEvent(QMoveEvent*) {
    handleGeometryChanged();
}

}
