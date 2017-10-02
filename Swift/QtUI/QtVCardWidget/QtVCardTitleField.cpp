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

#include <Swift/QtUI/QtVCardWidget/QtVCardTitleField.h>

#include <boost/algorithm/string.hpp>

#include <QGridLayout>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardTitleField::QtVCardTitleField(QWidget* parent, QGridLayout *layout, bool editable) :
    QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Title"), false, false), titleLineEdit(nullptr) {
    connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardTitleField::~QtVCardTitleField() {
    disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardTitleField::setupContentWidgets() {
    titleLineEdit = new QtResizableLineEdit(this);
    getGridLayout()->addWidget(titleLineEdit, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);
    getTagComboBox()->hide();
    childWidgets << titleLineEdit;
}

bool QtVCardTitleField::isEmpty() const {
    return titleLineEdit->text().isEmpty();
}

void QtVCardTitleField::setTitle(const std::string& title) {
    titleLineEdit->setText(P2QSTRING(title));
}

std::string QtVCardTitleField::getTitle() const {
    return Q2PSTRING(titleLineEdit->text());
}

void QtVCardTitleField::handleEditibleChanged(bool isEditable) {
    assert(titleLineEdit);

    titleLineEdit->setEditable(isEditable);
    titleLineEdit->setStyleSheet(isEditable ? "" : "QLineEdit { border: none; background: transparent; }");
}

}
