/*
 * Copyright (c) 2012-2014 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtVCardWidget/QtVCardAddressField.h>

#include <QGridLayout>

#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardAddressField::QtVCardAddressField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Address")), streetLineEdit(NULL), poboxLineEdit(NULL), addressextLineEdit(NULL), cityLineEdit(NULL), pocodeLineEdit(NULL), regionLineEdit(NULL), countryLineEdit(NULL), textFieldGridLayout(NULL), textFieldGridLayoutItem(NULL), deliveryTypeLabel(NULL), domesticRadioButton(NULL), internationalRadioButton(NULL), buttonGroup(NULL) {
	connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardAddressField::~QtVCardAddressField() {
	disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardAddressField::setupContentWidgets() {
	textFieldGridLayout = new QGridLayout();

	streetLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(streetLineEdit, 0, 0, Qt::AlignVCenter);

	poboxLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(poboxLineEdit, 0, 1, Qt::AlignVCenter);

	addressextLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(addressextLineEdit, 1, 0, Qt::AlignVCenter);

	cityLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(cityLineEdit, 2, 0, Qt::AlignVCenter);

	pocodeLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(pocodeLineEdit, 2, 1, Qt::AlignVCenter);

	regionLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(regionLineEdit, 3, 0, Qt::AlignVCenter);

	countryLineEdit = new QtResizableLineEdit(this);
	textFieldGridLayout->addWidget(countryLineEdit, 4, 0, Qt::AlignVCenter);
	textFieldGridLayout->setVerticalSpacing(2);
	getGridLayout()->addLayout(textFieldGridLayout, getGridLayout()->rowCount()-1, 2, 5, 2, Qt::AlignVCenter);
	textFieldGridLayoutItem = getGridLayout()->itemAtPosition(getGridLayout()->rowCount()-1, 2);

#if QT_VERSION >= 0x040700
	streetLineEdit->setPlaceholderText(tr("Street"));
	poboxLineEdit->setPlaceholderText(tr("PO Box"));
	addressextLineEdit->setPlaceholderText(tr("Address Extension"));
	cityLineEdit->setPlaceholderText(tr("City"));
	pocodeLineEdit->setPlaceholderText(tr("Postal Code"));
	regionLineEdit->setPlaceholderText(tr("Region"));
	countryLineEdit->setPlaceholderText(tr("Country"));
#endif

	deliveryTypeLabel = new QtElidingLabel(this);
	deliveryTypeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	getGridLayout()->addWidget(deliveryTypeLabel, getGridLayout()->rowCount()-3, 4, Qt::AlignVCenter);

	domesticRadioButton = new QRadioButton(tr("Domestic Delivery"), this);
	getGridLayout()->addWidget(domesticRadioButton, getGridLayout()->rowCount()-2, 4, Qt::AlignVCenter);

	internationalRadioButton = new QRadioButton(tr("International Delivery"), this);
	getGridLayout()->addWidget(internationalRadioButton, getGridLayout()->rowCount()-1, 4, Qt::AlignVCenter);

	buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(domesticRadioButton);
	buttonGroup->addButton(internationalRadioButton);

	setTabOrder(internationalRadioButton, getTagComboBox());
	getTagComboBox()->addTag("postal", tr("Postal"));
	getTagComboBox()->addTag("parcel", tr("Parcel"));

	QtVCardHomeWork::setTagComboBox(getTagComboBox());

	textFields << streetLineEdit << poboxLineEdit << addressextLineEdit << cityLineEdit << pocodeLineEdit << regionLineEdit << countryLineEdit;
	childWidgets << deliveryTypeLabel << domesticRadioButton << internationalRadioButton;
}

void QtVCardAddressField::customCleanup() {
	foreach(QWidget* widget, textFields) {
		widget->hide();
		textFieldGridLayout->removeWidget(widget);
	}
	getGridLayout()->removeItem(textFieldGridLayoutItem);
}



bool QtVCardAddressField::isEmpty() const {
	return streetLineEdit->text().isEmpty() &&
			poboxLineEdit->text().isEmpty() &&
			addressextLineEdit->text().isEmpty() &&
			cityLineEdit->text().isEmpty() &&
			pocodeLineEdit->text().isEmpty() &&
			regionLineEdit->text().isEmpty() &&
			countryLineEdit->text().isEmpty();
}

void QtVCardAddressField::setAddress(const VCard::Address& address) {
	setPreferred(address.isPreferred);
	setHome(address.isHome);
	setWork(address.isWork);
	getTagComboBox()->setTag("postal", address.isPostal);
	getTagComboBox()->setTag("parcel", address.isParcel);
	domesticRadioButton->setChecked(address.deliveryType == VCard::DomesticDelivery);
	internationalRadioButton->setChecked(address.deliveryType == VCard::InternationalDelivery);
	streetLineEdit->setText(P2QSTRING(address.street));
	poboxLineEdit->setText(P2QSTRING(address.poBox));
	addressextLineEdit->setText(P2QSTRING(address.addressExtension));
	cityLineEdit->setText(P2QSTRING(address.locality));
	pocodeLineEdit->setText(P2QSTRING(address.postalCode));
	regionLineEdit->setText(P2QSTRING(address.region));
	countryLineEdit->setText(P2QSTRING(address.country));
}

VCard::Address QtVCardAddressField::getAddress() const {
	VCard::Address address;
	address.isPreferred = getPreferred();
	address.isHome = getHome();
	address.isWork = getWork();
	address.deliveryType = domesticRadioButton->isChecked() ? VCard::DomesticDelivery : (internationalRadioButton->isChecked() ? VCard::InternationalDelivery : VCard::None);
	address.isPostal = getTagComboBox()->isTagSet("postal");
	address.isParcel = getTagComboBox()->isTagSet("parcel");
	address.street = Q2PSTRING(streetLineEdit->text());
	address.poBox = Q2PSTRING(poboxLineEdit->text());
	address.addressExtension = Q2PSTRING(addressextLineEdit->text());
	address.locality = Q2PSTRING(cityLineEdit->text());
	address.postalCode = Q2PSTRING(pocodeLineEdit->text());
	address.region = Q2PSTRING(regionLineEdit->text());
	address.country = Q2PSTRING(countryLineEdit->text());
	return address;
}

void QtVCardAddressField::handleEditibleChanged(bool isEditable) {
	assert(streetLineEdit);
	assert(poboxLineEdit);
	assert(addressextLineEdit);
	assert(cityLineEdit);
	assert(pocodeLineEdit);
	assert(regionLineEdit);
	assert(countryLineEdit);
	assert(deliveryTypeLabel);
	assert(domesticRadioButton);
	assert(internationalRadioButton);

	streetLineEdit->setEditable(isEditable);
	poboxLineEdit->setEditable(isEditable);
	addressextLineEdit->setEditable(isEditable);
	cityLineEdit->setEditable(isEditable);
	pocodeLineEdit->setEditable(isEditable);
	regionLineEdit->setEditable(isEditable);
	countryLineEdit->setEditable(isEditable);

	deliveryTypeLabel->setText(buttonGroup->checkedButton() == 0 ? "" : buttonGroup->checkedButton()->text());
	deliveryTypeLabel->setVisible(!isEditable);

	domesticRadioButton->setVisible(isEditable);
	internationalRadioButton->setVisible(isEditable);

	foreach (QWidget* widget, textFields) {
		QtResizableLineEdit* lineEdit;
		if ((lineEdit = dynamic_cast<QtResizableLineEdit*>(widget))) {
			lineEdit->setVisible(isEditable ? true : !lineEdit->text().isEmpty());
			lineEdit->setStyleSheet(isEditable ? "" : "QLineEdit { border: none; background: transparent; }");
		}
	}
}

}
