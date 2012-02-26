/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtResizableLineEdit.h"

namespace Swift {

QtResizableLineEdit::QtResizableLineEdit(QWidget* parent) :
	QLineEdit(parent) {
	connect(this, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	int marginHeight = 6;
	setMaximumHeight(fontMetrics().height() + marginHeight);
}

QtResizableLineEdit::~QtResizableLineEdit() {
}

bool QtResizableLineEdit::isEditable() const {
	return editable;
}
void QtResizableLineEdit::setEditable(const bool editable) {
	this->editable = editable;
	if (editable) {
		setReadOnly(false);
	} else {
		setReadOnly(true);
	}
}


QSize QtResizableLineEdit::sizeHint() const {
	int horizontalMargin = 10;
#if QT_VERSION >= 0x040700
	int w = fontMetrics().boundingRect(text().isEmpty() ? placeholderText() : text()).width() + horizontalMargin;
#else
	int w = fontMetrics().boundingRect(text().isEmpty() ? QString("   ") : text()).width() + horizontalMargin;
#endif
	return QSize(w, height());
}

void QtResizableLineEdit::textChanged(QString) {
	updateGeometry();
}

}
