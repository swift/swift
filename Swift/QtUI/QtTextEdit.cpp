#include "QtTextEdit.h"

#include <QFontMetrics>
#include <QKeyEvent>

namespace Swift {

QtTextEdit::QtTextEdit(QWidget* parent) : QTextEdit(parent){
	connect(this, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
	handleTextChanged();
};

void QtTextEdit::keyPressEvent(QKeyEvent* event) {
	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
		&& (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier)) {
		emit returnPressed();
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



