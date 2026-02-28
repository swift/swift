/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtClickableLabel.h>

namespace Swift {

QtClickableLabel::QtClickableLabel(QWidget* parent) : QLabel(parent) {
}

void QtClickableLabel::mousePressEvent(QMouseEvent*) {
    emit clicked();
}

}
