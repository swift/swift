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

#include <Swift/QtUI/QtVCardWidget/QtVCardBirthdayField.h>

#include <boost/algorithm/string.hpp>

#include <QGridLayout>
#include <QHBoxLayout>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardBirthdayField::QtVCardBirthdayField(QWidget* parent, QGridLayout *layout, bool editable) :
    QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Birthday"), false, false), birthdayLabel(nullptr), birthdayDateEdit(nullptr) {
    connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardBirthdayField::~QtVCardBirthdayField() {
    disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardBirthdayField::setupContentWidgets() {
    birthdayLabel = new QLabel(this);
    birthdayLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    birthdayDateEdit = new QDateEdit(this);
    birthdayDateEdit->setCalendarPopup(true);

    QHBoxLayout* birthdayLayout = new QHBoxLayout();
    birthdayLayout->addWidget(birthdayLabel);
    birthdayLayout->addWidget(birthdayDateEdit);

    getGridLayout()->addLayout(birthdayLayout, getGridLayout()->rowCount()-1, 2, Qt::AlignVCenter);

    getTagComboBox()->hide();
    birthdayLabel->hide();
    childWidgets << birthdayLabel << birthdayDateEdit;
}

bool QtVCardBirthdayField::isEmpty() const {
    return false;
}

void QtVCardBirthdayField::setBirthday(const boost::posix_time::ptime& birthday) {
    birthdayDateEdit->setDate(B2QDATE(birthday).date());
}

boost::posix_time::ptime QtVCardBirthdayField::getBirthday() const {
    return boost::posix_time::from_time_t(QDateTime(birthdayDateEdit->date()).toTime_t());
}

void QtVCardBirthdayField::handleEditibleChanged(bool isEditable) {
    birthdayLabel->setText(birthdayDateEdit->date().toString(Qt::DefaultLocaleLongDate));
    birthdayDateEdit->setVisible(isEditable);
    birthdayLabel->setVisible(!isEditable);
}

}
