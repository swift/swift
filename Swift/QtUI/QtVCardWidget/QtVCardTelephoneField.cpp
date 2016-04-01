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

#include <Swift/QtUI/QtVCardWidget/QtVCardTelephoneField.h>

#include <QGridLayout>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardTelephoneField::QtVCardTelephoneField(QWidget* parent, QGridLayout *layout, bool editable) :
    QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Telephone")), telephoneLineEdit(nullptr) {
    connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardTelephoneField::~QtVCardTelephoneField() {
    disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardTelephoneField::setupContentWidgets() {
    telephoneLineEdit = new QtResizableLineEdit(this);
#if QT_VERSION >= 0x040700
    telephoneLineEdit->setPlaceholderText(tr("0118 999 881 999 119 7253"));
#endif
    getGridLayout()->addWidget(telephoneLineEdit, getGridLayout()->rowCount()-1, 2, 1, 2, Qt::AlignVCenter);
    setTabOrder(telephoneLineEdit, getTagComboBox());
    QtVCardHomeWork::setTagComboBox(getTagComboBox());

    getTagComboBox()->addTag("voice", QObject::tr("Voice"));
    getTagComboBox()->addTag("fax",    QObject::tr("Fax"));
    getTagComboBox()->addTag("pager", QObject::tr("Pager"));
    getTagComboBox()->addTag("msg",    QObject::tr("Voice Messaging"));
    getTagComboBox()->addTag("cell", QObject::tr("Cell"));
    getTagComboBox()->addTag("video", QObject::tr("Video"));
    getTagComboBox()->addTag("bbs",    QObject::tr("Bulletin Board System"));
    getTagComboBox()->addTag("modem", QObject::tr("Modem"));
    getTagComboBox()->addTag("isdn", QObject::tr("ISDN"));
    getTagComboBox()->addTag("pcs",    QObject::tr("Personal Communication Services"));

    childWidgets << telephoneLineEdit;
}

bool QtVCardTelephoneField::isEmpty() const {
    return telephoneLineEdit->text().isEmpty();
}

void QtVCardTelephoneField::setTelephone(const VCard::Telephone& telephone) {
    setPreferred(telephone.isPreferred);
    setHome(telephone.isHome);
    setWork(telephone.isWork);

    telephoneLineEdit->setText(P2QSTRING(telephone.number));

    getTagComboBox()->setTag("voice", telephone.isVoice);
    getTagComboBox()->setTag("fax", telephone.isFax);
    getTagComboBox()->setTag("pager", telephone.isPager);
    getTagComboBox()->setTag("msg", telephone.isMSG);
    getTagComboBox()->setTag("cell", telephone.isCell);
    getTagComboBox()->setTag("video", telephone.isVideo);
    getTagComboBox()->setTag("bbs", telephone.isBBS);
    getTagComboBox()->setTag("modem", telephone.isModem);
    getTagComboBox()->setTag("isdn", telephone.isISDN);
    getTagComboBox()->setTag("pcs", telephone.isPCS);
}

VCard::Telephone QtVCardTelephoneField::getTelephone() const {
    VCard::Telephone telephone;

    telephone.number = Q2PSTRING(telephoneLineEdit->text());

    telephone.isPreferred = getPreferred();
    telephone.isHome = getHome();
    telephone.isWork = getWork();

    telephone.isVoice = getTagComboBox()->isTagSet("voice");
    telephone.isFax = getTagComboBox()->isTagSet("fax");
    telephone.isPager = getTagComboBox()->isTagSet("pager");
    telephone.isMSG = getTagComboBox()->isTagSet("msg");
    telephone.isCell = getTagComboBox()->isTagSet("cell");
    telephone.isVideo = getTagComboBox()->isTagSet("video");
    telephone.isBBS = getTagComboBox()->isTagSet("bbs");
    telephone.isModem = getTagComboBox()->isTagSet("modem");
    telephone.isISDN = getTagComboBox()->isTagSet("isdn");
    telephone.isPCS = getTagComboBox()->isTagSet("pcs");

    return telephone;
}

void QtVCardTelephoneField::handleEditibleChanged(bool isEditable) {
    assert(telephoneLineEdit);

    telephoneLineEdit->setEditable(isEditable);
    telephoneLineEdit->setStyleSheet(isEditable ? "" : "QLineEdit { border: none; background: transparent; }");
}

}
