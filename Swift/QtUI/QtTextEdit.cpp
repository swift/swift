/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtTextEdit.h"

#include <QFontMetrics>
#include <QKeyEvent>

namespace Swift {

QtTextEdit::QtTextEdit(QWidget* parent) : QTextEdit(parent){
	connect(this, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
	handleTextChanged();
};

void QtTextEdit::keyPressEvent(QKeyEvent* event) {
	int key = event->key();
	Qt::KeyboardModifiers modifiers = event->modifiers();
	if ((key == Qt::Key_Enter || key == Qt::Key_Return)
		&& (modifiers == Qt::NoModifier || modifiers == Qt::KeypadModifier)) {
		emit returnPressed();
	} else if (((key == Qt::Key_PageUp || key == Qt::Key_PageDown) && modifiers == Qt::ShiftModifier)
			   || (key == Qt::Key_C && modifiers == Qt::ControlModifier && textCursor().selectedText().isEmpty())
			   || (key == Qt::Key_W && modifiers == Qt::ControlModifier)) {
		emit unhandledKeyPressEvent(event);
	} else {
		QTextEdit::keyPressEvent(event);
	}
}

void QtTextEdit::handleTextChanged() {
	setMaximumSize(QSize(maximumWidth(), sizeHint().height()));
	updateGeometry();
}

QSize QtTextEdit::sizeHint() const {
	QFontMetrics inputMetrics(currentFont());
	QRect boundingRect = inputMetrics.boundingRect(geometry(), Qt::TextWordWrap, toPlainText());
	int height = boundingRect.height() + inputMetrics.height();
	return QSize(width(), height);
	//int numberOfLines = 1;
	//int lineHeight = inputMetrics.lineSpacing();
	//return QSize(QTextEdit::sizeHint().width(), lineHeight * numberOfLines);
}

}



