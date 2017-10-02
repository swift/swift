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

#include <Swift/QtUI/QtVCardWidget/QtVCardWidget.h>

#include <QLineEdit>
#include <QMenu>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardAddressField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardAddressLabelField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardBirthdayField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardDescriptionField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardInternetEMailField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardJIDField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardOrganizationField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardRoleField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardTelephoneField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardTitleField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardURLField.h>
#include <Swift/QtUI/QtVCardWidget/ui_QtVCardWidget.h>

namespace Swift {

QtVCardWidget::QtVCardWidget(QWidget* parent) :
    QWidget(parent),
    ui(new ::Ui::QtVCardWidget) {
    ui->setupUi(this);

    ui->cardFields->setColumnStretch(0,0);
    ui->cardFields->setColumnStretch(1,0);
    ui->cardFields->setColumnStretch(2,2);
    ui->cardFields->setColumnStretch(3,1);
    ui->cardFields->setColumnStretch(4,2);
    menu = new QMenu(this);

    toolButton = new QToolButton(this);
    toolButton->setText(tr("Add Field"));
    toolButton->setArrowType(Qt::NoArrow);
    toolButton->setAutoRaise(false);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->hide();
    toolButton->setMenu(menu);

    addFieldType(menu, std::make_shared<QtVCardInternetEMailField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardTelephoneField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardAddressField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardAddressLabelField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardBirthdayField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardJIDField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardDescriptionField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardRoleField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardTitleField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardOrganizationField::FieldInfo>());
    addFieldType(menu, std::make_shared<QtVCardURLField::FieldInfo>());

    setEditable(false);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

QtVCardWidget::~QtVCardWidget() {
    delete ui;
}

QSize QtVCardWidget::sizeHint() const {
    QSize newSizeHint = ui->photoAndName->sizeHint();

    // use mininmal size that does not require scrolling
    QSize fieldsWidgetSize = ui->scrollArea->widget()->minimumSize();
    fieldsWidgetSize.setWidth(ui->scrollArea->widget()->sizeHint().width());

    newSizeHint += QSize(0, ui->line->height());

    newSizeHint = QSize(std::max(newSizeHint.width(), fieldsWidgetSize.width()), newSizeHint.height() + fieldsWidgetSize.height());

    // add layout margin
    newSizeHint += QSize(layout()->contentsMargins().left() + layout()->contentsMargins().right(), layout()->contentsMargins().top() + layout()->contentsMargins().bottom());

    // the spaceing before and after the line between the profile header and its fields
    newSizeHint += QSize(0, layout()->spacing() * 2);

    return newSizeHint;
}

bool QtVCardWidget::isEditable() const {
    return editable;
}

void QtVCardWidget::setEditable(bool editable) {
    this->editable = editable;

    ui->photoAndName->setProperty("editable", QVariant(editable));

    for (auto field : fields) {
        field->setEditable(editable);
    }
    toolButton->setVisible(editable);

    editableChanged(editable);
}

void QtVCardWidget::setVCard(VCard::ref vcard) {
    clearFields();
    this->vcard = std::make_shared<VCard>(*vcard);
    ui->photoAndName->setFormattedName(P2QSTRING(vcard->getFullName()));
    ui->photoAndName->setNickname(P2QSTRING(vcard->getNickname()));
    ui->photoAndName->setPrefix(P2QSTRING(vcard->getPrefix()));
    ui->photoAndName->setGivenName(P2QSTRING(vcard->getGivenName()));
    ui->photoAndName->setMiddleName(P2QSTRING(vcard->getMiddleName()));
    ui->photoAndName->setFamilyName(P2QSTRING(vcard->getFamilyName()));
    ui->photoAndName->setSuffix(P2QSTRING(vcard->getSuffix()));
    ui->photoAndName->setAvatar(vcard->getPhoto(), vcard->getPhotoType());

    for (const auto& address : vcard->getEMailAddresses()) {
        if (address.isInternet) {
            QtVCardInternetEMailField* internetEmailField = new QtVCardInternetEMailField(this, ui->cardFields);
            internetEmailField->initialize();
            internetEmailField->setInternetEMailAddress(address);
            appendField(internetEmailField);
        }
    }

    for (const auto& telephone : vcard->getTelephones()) {
        QtVCardTelephoneField* telField = new QtVCardTelephoneField(this, ui->cardFields);
        telField->initialize();
        telField->setTelephone(telephone);
        appendField(telField);
    }

    for (const auto& address : vcard->getAddresses()) {
        QtVCardAddressField* addressField = new QtVCardAddressField(this, ui->cardFields);
        addressField->initialize();
        addressField->setAddress(address);
        appendField(addressField);
    }

    for (const auto& label : vcard->getAddressLabels()) {
        QtVCardAddressLabelField* addressLabelField = new QtVCardAddressLabelField(this, ui->cardFields);
        addressLabelField->initialize();
        addressLabelField->setAddressLabel(label);
        appendField(addressLabelField);
    }

    if (!vcard->getBirthday().is_not_a_date_time()) {
        QtVCardBirthdayField* bdayField = new QtVCardBirthdayField(this, ui->cardFields);
        bdayField->initialize();
        bdayField->setBirthday(vcard->getBirthday());
        appendField(bdayField);
    }

    for (const auto& jid : vcard->getJIDs()) {
        QtVCardJIDField* jidField = new QtVCardJIDField(this, ui->cardFields);
        jidField->initialize();
        jidField->setJID(jid);
        appendField(jidField);
    }

    if (!vcard->getDescription().empty()) {
        QtVCardDescriptionField* descField = new QtVCardDescriptionField(this, ui->cardFields);
        descField->initialize();
        descField->setDescription(vcard->getDescription());
        appendField(descField);
    }

    for (const auto& org : vcard->getOrganizations()) {
        QtVCardOrganizationField* orgField = new QtVCardOrganizationField(this, ui->cardFields);
        orgField->initialize();
        orgField->setOrganization(org);
        appendField(orgField);
    }

    for (const auto& role : vcard->getRoles()) {
        QtVCardRoleField* roleField = new QtVCardRoleField(this, ui->cardFields);
        roleField->initialize();
        roleField->setRole(role);
        appendField(roleField);
    }

    for (const auto& title : vcard->getTitles()) {
        QtVCardTitleField* titleField = new QtVCardTitleField(this, ui->cardFields);
        titleField->initialize();
        titleField->setTitle(title);
        appendField(titleField);
    }

    for (const auto& url : vcard->getURLs()) {
        QtVCardURLField* urlField = new QtVCardURLField(this, ui->cardFields);
        urlField->initialize();
        urlField->setURL(url);
        appendField(urlField);
    }

    relayoutToolButton();
    setEditable(editable);
}

VCard::ref QtVCardWidget::getVCard() {
    clearEmptyFields();
    vcard->setFullName(Q2PSTRING(ui->photoAndName->getFormattedName()));
    vcard->setNickname(Q2PSTRING(ui->photoAndName->getNickname()));
    vcard->setPrefix(Q2PSTRING(ui->photoAndName->getPrefix()));
    vcard->setGivenName(Q2PSTRING(ui->photoAndName->getGivenName()));
    vcard->setMiddleName(Q2PSTRING(ui->photoAndName->getMiddleName()));
    vcard->setFamilyName(Q2PSTRING(ui->photoAndName->getFamilyName()));
    vcard->setSuffix(Q2PSTRING(ui->photoAndName->getSuffix()));
    vcard->setPhoto(ui->photoAndName->getAvatarData());
    vcard->setPhotoType(ui->photoAndName->getAvatarType());

    vcard->clearEMailAddresses();
    vcard->clearJIDs();
    vcard->clearURLs();
    vcard->clearTelephones();
    vcard->clearRoles();
    vcard->clearTitles();
    vcard->clearOrganizations();
    vcard->clearAddresses();
    vcard->clearAddressLabels();


    QtVCardBirthdayField* bdayField = nullptr;
    QtVCardDescriptionField* descriptionField = nullptr;

    for (auto field : fields) {
        QtVCardInternetEMailField* emailField;
        if ((emailField = dynamic_cast<QtVCardInternetEMailField*>(field))) {
            vcard->addEMailAddress(emailField->getInternetEMailAddress());
            continue;
        }

        QtVCardTelephoneField* telephoneField;
        if ((telephoneField = dynamic_cast<QtVCardTelephoneField*>(field))) {
            vcard->addTelephone(telephoneField->getTelephone());
            continue;
        }

        QtVCardAddressField* addressField;
        if ((addressField = dynamic_cast<QtVCardAddressField*>(field))) {
            vcard->addAddress(addressField->getAddress());
            continue;
        }

        QtVCardAddressLabelField* addressLabelField;
        if ((addressLabelField = dynamic_cast<QtVCardAddressLabelField*>(field))) {
            vcard->addAddressLabel(addressLabelField->getAddressLabel());
            continue;
        }

        if ((bdayField = dynamic_cast<QtVCardBirthdayField*>(field))) {
            continue;
        }

        QtVCardJIDField* jidField;
        if ((jidField = dynamic_cast<QtVCardJIDField*>(field))) {
            vcard->addJID(jidField->getJID());
            continue;
        }

        if ((descriptionField = dynamic_cast<QtVCardDescriptionField*>(field))) {
            continue;
        }

        QtVCardOrganizationField* orgField;
        if ((orgField = dynamic_cast<QtVCardOrganizationField*>(field))) {
            vcard->addOrganization(orgField->getOrganization());
            continue;
        }

        QtVCardRoleField* roleField;
        if ((roleField = dynamic_cast<QtVCardRoleField*>(field))) {
            vcard->addRole(roleField->getRole());
            continue;
        }

        QtVCardTitleField* titleField;
        if ((titleField = dynamic_cast<QtVCardTitleField*>(field))) {
            vcard->addTitle(titleField->getTitle());
            continue;
        }

        QtVCardURLField* urlField;
        if ((urlField = dynamic_cast<QtVCardURLField*>(field))) {
            vcard->addURL(urlField->getURL());
            continue;
        }
    }

    if (bdayField) {
        vcard->setBirthday(bdayField->getBirthday());
    } else {
        vcard->setBirthday(boost::posix_time::ptime());
    }

    if (descriptionField) {
        vcard->setDescription(descriptionField->getDescription());
    } else {
        vcard->setDescription("");
    }

    return vcard;
}

void QtVCardWidget::addField() {
    QAction* action = nullptr;
    if ((action = dynamic_cast<QAction*>(sender()))) {
        std::shared_ptr<QtVCardFieldInfo> fieldInfo = actionFieldInfo[action];
        QWidget* newField = fieldInfo->createFieldInstance(this, ui->cardFields, true);
        QtVCardGeneralField* newGeneralField = dynamic_cast<QtVCardGeneralField*>(newField);
        if (newGeneralField) {
            newGeneralField->initialize();
            appendField(newGeneralField);
            relayoutToolButton();
        }
    }
}

void QtVCardWidget::removeField(QtVCardGeneralField *field) {
    int sameFields = 0;
    QtVCardGeneralField* fieldToChange = nullptr;
    for (auto vcardField : fields) {
        if ((vcardField != field) && (typeid(*vcardField) == typeid(*field))) {
            sameFields++;
            fieldToChange = vcardField;
        }
    }

    if ((sameFields == 1) && fieldToChange) {
        fieldToChange->setStarVisible(false);
    }

    fields.remove(field);
    delete field;
}

void QtVCardWidget::addFieldType(QMenu* menu, std::shared_ptr<QtVCardFieldInfo> fieldType) {
    if (!fieldType->getMenuName().isEmpty()) {
        QAction* action = new QAction(tr("Add %1").arg(fieldType->getMenuName()), this);
        actionFieldInfo[action] = fieldType;
        connect(action, SIGNAL(triggered()), this, SLOT(addField()));
        menu->addAction(action);
    }
}

int QtVCardWidget::fieldTypeInstances(std::shared_ptr<QtVCardFieldInfo> fieldType) {
    int instances = 0;
    for (int n = 0; n < ui->cardFields->count(); n++) {
        if (fieldType->testInstance(ui->cardFields->itemAt(n)->widget())) instances++;
    }
    return instances;
}

void layoutDeleteChildren(QLayout *layout) {
    while(layout->count() > 0) {
        QLayoutItem* child;
        if ((child = layout->takeAt(0)) != nullptr) {
            if (child->layout()) {
                layoutDeleteChildren(child->layout());
            }
            if (dynamic_cast<QToolButton*>(child->widget())) {
                delete child;
                break;
            }
            delete child->widget();
            delete child;
        }
    }
}

void QtVCardWidget::clearFields() {
    for (auto field : fields) {
        delete field;
    }
    fields.clear();

    assert(ui->cardFields->count() >= 0);
    layoutDeleteChildren(ui->cardFields);
}

void QtVCardWidget::clearEmptyFields() {
    std::vector<QtVCardGeneralField*> items_to_remove;
    for (auto field : fields) {
        if (field->property("empty").isValid() && field->property("empty").toBool()) {
            ui->cardFields->removeWidget(field);
            items_to_remove.push_back(field);
            delete field;
        }
    }

    for (auto field : items_to_remove) {
        fields.remove(field);
    }
}

void QtVCardWidget::appendField(QtVCardGeneralField *field) {
    connect(field, SIGNAL(deleteField(QtVCardGeneralField*)), SLOT(removeField(QtVCardGeneralField*)));

    QtVCardGeneralField* fieldToChange = nullptr;
    for (auto vcardField : fields) {
        if (typeid(*vcardField) == typeid(*field)) {
            fieldToChange = vcardField;
            break;
        }
    }

    if (fieldToChange) {
        fieldToChange->setStarVisible(true);
        field->setStarVisible(true);
    }

    fields.push_back(field);
}

void QtVCardWidget::relayoutToolButton() {
    ui->cardFields->addWidget(toolButton, ui->cardFields->rowCount(), ui->cardFields->columnCount()-2, 1, 1, Qt::AlignRight);
}

}
