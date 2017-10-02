/*
 * Copyright (c) 2012-2014 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtVCardDescriptionField.h>

#include <boost/algorithm/string.hpp>

#include <QFontMetrics>
#include <QGridLayout>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardDescriptionField::QtVCardDescriptionField(QWidget* parent, QGridLayout *layout, bool editable) :
    QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Description"), false, false), descriptionPlainTextEdit(nullptr) {
    connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardDescriptionField::~QtVCardDescriptionField() {
    disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardDescriptionField::setupContentWidgets() {
    descriptionPlainTextEdit = new QPlainTextEdit(this);
    descriptionPlainTextEdit->setMinimumHeight(70);
    getGridLayout()->addWidget(descriptionPlainTextEdit, getGridLayout()->rowCount()-1, 2, 2, 2, Qt::AlignVCenter);
    getTagComboBox()->hide();
    childWidgets << descriptionPlainTextEdit;
}

bool QtVCardDescriptionField::isEmpty() const {
    return descriptionPlainTextEdit->toPlainText().isEmpty();
}

void QtVCardDescriptionField::setDescription(const std::string& description) {
    descriptionPlainTextEdit->setPlainText(P2QSTRING(description));
}

std::string QtVCardDescriptionField::getDescription() const {
    return Q2PSTRING(descriptionPlainTextEdit->toPlainText());
}

void QtVCardDescriptionField::handleEditibleChanged(bool isEditable) {
    assert(descriptionPlainTextEdit);

    if (isEditable) {
        descriptionPlainTextEdit->setMinimumHeight(70);
    } else {
        QFontMetrics inputMetrics(descriptionPlainTextEdit->document()->defaultFont());
        QRect horizontalBounds = contentsRect().adjusted(0,0,0,9999);
        QRect boundingRect = inputMetrics.boundingRect(horizontalBounds, Qt::TextWordWrap, descriptionPlainTextEdit->toPlainText() + "A");
        int left, top, right, bottom;
        getContentsMargins(&left, &top, &right, &bottom);
        int height = boundingRect.height() + top + bottom + inputMetrics.height();
        descriptionPlainTextEdit->setMinimumHeight(height > 70 ? 70 : height);
    }
    descriptionPlainTextEdit->setReadOnly(!isEditable);
    descriptionPlainTextEdit->setStyleSheet(isEditable ? "" : "QPlainTextEdit { background: transparent; }");
}

}
