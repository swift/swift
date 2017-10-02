/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtLineEdit.h>

#include <QKeyEvent>

namespace Swift {
QtLineEdit::QtLineEdit(QWidget* parent) : QLineEdit(parent) {

}

void QtLineEdit::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        emit escapePressed();
    }
    QLineEdit::keyPressEvent(event);
}

}
