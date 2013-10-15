/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtVCardTitleField.h"

#include <QGridLayout>
#include <boost/algorithm/string.hpp>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardTitleField::QtVCardTitleField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Title"), false, false) {
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
