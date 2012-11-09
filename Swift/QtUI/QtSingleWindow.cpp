/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtSingleWindow.h>

#include <Swift/QtUI/QtSettingsProvider.h>

namespace Swift {

static const QString SINGLE_WINDOW_GEOMETRY = QString("SINGLE_WINDOW_GEOMETRY");

QtSingleWindow::QtSingleWindow(QtSettingsProvider* settings) : QSplitter() {
	settings_ = settings;
	QVariant geometryVariant = settings_->getQSettings()->value(SINGLE_WINDOW_GEOMETRY);
	if (geometryVariant.isValid()) {
		restoreGeometry(geometryVariant.toByteArray());
	}


}

QtSingleWindow::~QtSingleWindow() {

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
