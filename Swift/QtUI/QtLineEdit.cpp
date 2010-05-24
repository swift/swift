/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/QtLineEdit.h"

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
