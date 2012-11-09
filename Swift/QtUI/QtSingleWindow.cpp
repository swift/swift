/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtSingleWindow.h>

#include <Swift/QtUI/QtSettingsProvider.h>
#include <Swiften/Base/foreach.h>

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

void QtSingleWindow::handleSplitterMoved(int, int) {
	QList<QVariant> variantValues;
	QList<int> intValues = sizes();
	foreach (int value, intValues) {
		variantValues.append(QVariant(value));
	}
	settings_->getQSettings()->setValue(SINGLE_WINDOW_SPLITS, QVariant(variantValues));
}

void QtSingleWindow::restoreSplitters() {
	QList<QVariant> variantValues = settings_->getQSettings()->value(SINGLE_WINDOW_SPLITS).toList();
	QList<int> intValues;
	foreach (QVariant value, variantValues) {
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

void QtSingleWindow::resizeEvent(QResizeEvent*) {
	handleGeometryChanged();
}

void QtSingleWindow::moveEvent(QMoveEvent*) {
	handleGeometryChanged();
}

}
