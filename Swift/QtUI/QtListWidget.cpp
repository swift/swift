/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtListWidget.h>

#include <QListWidget>

namespace Swift {

QtListWidget::QtListWidget(QWidget* parent) : QListWidget(parent) {
}

void QtListWidget::resizeEvent(QResizeEvent*) {
    emit onResize();
}

}
