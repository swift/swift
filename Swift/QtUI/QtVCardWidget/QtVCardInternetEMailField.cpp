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

#include <Swift/QtUI/QtVCardWidget/QtVCardInternetEMailField.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QTextDocument>

#include <Swiften/Base/Log.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtVCardInternetEMailField::QtVCardInternetEMailField(QWidget* parent, QGridLayout *layout, bool editable) :
    QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("E-Mail")), emailLineEdit(nullptr), emailLabel(nullptr) {
    connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardInternetEMailField::~QtVCardInternetEMailField() {
    disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardInternetEMailField::setupContentWidgets() {
    emailLabel = new QLabel(this);
    emailLabel->setOpenExternalLinks(true);
    emailLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    emailLineEdit = new QtResizableLineEdit(this);
#if QT_VERSION >= 0x040700
    emailLineEdit->setPlaceholderText(tr("alice@wonderland.lit"));
#endif
    QHBoxLayout* emailLayout = new QHBoxLayout();
    emailLayout->addWidget(emailLabel);
    emailLayout->addWidget(emailLineEdit);
    getGridLayout()->addLayout(emailLayout, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);
    setTabOrder(emailLineEdit, getTagComboBox());
    QtVCardHomeWork::setTagComboBox(getTagComboBox());
    emailLabel->hide();
    childWidgets << emailLabel << emailLineEdit;
}

bool QtVCardInternetEMailField::isEmpty() const {
    return emailLineEdit->text().isEmpty();
}

void QtVCardInternetEMailField::setInternetEMailAddress(const VCard::EMailAddress& address) {
    assert(address.isInternet);
    setPreferred(address.isPreferred);
    setHome(address.isHome);
    setWork(address.isWork);
    emailLineEdit->setText(P2QSTRING(address.address));
}

VCard::EMailAddress QtVCardInternetEMailField::getInternetEMailAddress() const {
    VCard::EMailAddress address;
    address.isInternet = true;
    address.isPreferred = getPreferred();
    address.isHome = getHome();
    address.isWork = getWork();
    address.address = Q2PSTRING(emailLineEdit->text());
    return address;
}

void QtVCardInternetEMailField::handleEditibleChanged(bool isEditable) {
    assert(emailLineEdit);
    assert(emailLabel);

    if (isEditable) {
        emailLineEdit->show();
        emailLabel->hide();
    } else {
        emailLineEdit->hide();
        emailLabel->setText(QString("<a href=\"mailto:%1\">%1</a>").arg(QtUtilities::htmlEscape(emailLineEdit->text())));
        emailLabel->show();
    }
}

}
