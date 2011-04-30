/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtTextEdit.h>

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
	}
	else if (((key == Qt::Key_PageUp || key == Qt::Key_PageDown) && modifiers == Qt::ShiftModifier)
			   || (key == Qt::Key_C && modifiers == Qt::ControlModifier && textCursor().selectedText().isEmpty())
			   || (key == Qt::Key_W && modifiers == Qt::ControlModifier)
			   || (key == Qt::Key_PageUp && modifiers == Qt::ControlModifier)
			   || (key == Qt::Key_PageDown && modifiers == Qt::ControlModifier)
			   || (key == Qt::Key_Tab && modifiers == Qt::ControlModifier)
			   || (key == Qt::Key_A && modifiers == Qt::AltModifier)
			   || (key == Qt::Key_Tab)
	) {
		emit unhandledKeyPressEvent(event);
	}
	else if ((key == Qt::Key_Up)
			   || (key == Qt::Key_Down)
	){
		emit unhandledKeyPressEvent(event);
		QTextEdit::keyPressEvent(event);
	}
	else {
		QTextEdit::keyPressEvent(event);
	}
}

void QtTextEdit::handleTextChanged() {
	QSize previous(maximumSize());
	setMaximumSize(QSize(maximumWidth(), sizeHint().height()));
	if (previous != maximumSize()) {
		updateGeometry();
	}
}

QSize QtTextEdit::sizeHint() const {
	QFontMetrics inputMetrics(currentFont());
	QRect horizontalBounds = contentsRect().adjusted(0,0,0,9999);
	QRect boundingRect = inputMetrics.boundingRect(horizontalBounds, Qt::TextWordWrap, toPlainText() + "A");
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	int height = boundingRect.height() + top + bottom + inputMetrics.height();
	return QSize(width(), height);
	//int numberOfLines = 1;
	//int lineHeight = inputMetrics.lineSpacing();
	//return QSize(QTextEdit::sizeHint().width(), lineHeight * numberOfLines);
}

}



