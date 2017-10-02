/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swift/QtUI/QtVCardWidget/QtResizableLineEdit.h>

namespace Swift {

QtResizableLineEdit::QtResizableLineEdit(QWidget* parent) :
    QLineEdit(parent), editable(false) {
    connect(this, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
    setMinimumWidth(30);
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
    int verticalMargin = 6;
    QSize textDimensions;
#if QT_VERSION >= 0x040700
    textDimensions = fontMetrics().boundingRect(text().isEmpty() ? placeholderText() : text()).size();
#else
    textDimensions = fontMetrics().boundingRect(text().isEmpty() ? QString("   ") : text()).size();
#endif
    textDimensions.setWidth(textDimensions.width() + horizontalMargin);
    textDimensions.setHeight(textDimensions.height() + verticalMargin);
    return textDimensions;
}

void QtResizableLineEdit::textChanged(QString) {
    updateGeometry();
}

}
