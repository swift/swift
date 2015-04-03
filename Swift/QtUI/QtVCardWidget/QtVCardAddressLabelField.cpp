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

#include <Swift/QtUI/QtVCardWidget/QtVCardAddressLabelField.h>

#include <boost/algorithm/string.hpp>

#include <QGridLayout>

#include <Swift/QtUI/QtElidingLabel.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtVCardAddressLabelField::QtVCardAddressLabelField(QWidget* parent, QGridLayout *layout, bool editable) :
	QtVCardGeneralField(parent, layout, editable, layout->rowCount(), tr("Address Label")), addressLabelPlainTextEdit(NULL), deliveryTypeLabel(NULL), domesticRadioButton(NULL), internationalRadioButton(NULL), buttonGroup(NULL) {
	connect(this, SIGNAL(editableChanged(bool)), SLOT(handleEditibleChanged(bool)));
}

QtVCardAddressLabelField::~QtVCardAddressLabelField() {
	disconnect(this, SLOT(handleEditibleChanged(bool)));
}

void QtVCardAddressLabelField::setupContentWidgets() {
	addressLabelPlainTextEdit = new QPlainTextEdit(this);
	addressLabelPlainTextEdit->setTabChangesFocus(true);
	getGridLayout()->addWidget(addressLabelPlainTextEdit, getGridLayout()->rowCount()-1, 2, 3, 2, Qt::AlignVCenter);

	deliveryTypeLabel = new QtElidingLabel(this);
	deliveryTypeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
	getGridLayout()->addWidget(deliveryTypeLabel, getGridLayout()->rowCount()-2, 4, Qt::AlignVCenter);

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
	deliveryTypeLabel->hide();
	childWidgets << addressLabelPlainTextEdit << deliveryTypeLabel << domesticRadioButton << internationalRadioButton;
}

bool QtVCardAddressLabelField::isEmpty() const {
	return addressLabelPlainTextEdit->toPlainText().isEmpty();
}

void QtVCardAddressLabelField::setAddressLabel(const VCard::AddressLabel& addressLabel) {
	setPreferred(addressLabel.isPreferred);
	setHome(addressLabel.isHome);
	setWork(addressLabel.isWork);
	getTagComboBox()->setTag("postal", addressLabel.isPostal);
	getTagComboBox()->setTag("parcel", addressLabel.isParcel);
	domesticRadioButton->setChecked(addressLabel.deliveryType == VCard::DomesticDelivery);
	internationalRadioButton->setChecked(addressLabel.deliveryType == VCard::InternationalDelivery);
	std::string joinedLines = boost::algorithm::join(addressLabel.lines, "\n");
	addressLabelPlainTextEdit->setPlainText(P2QSTRING(joinedLines));
}

VCard::AddressLabel QtVCardAddressLabelField::getAddressLabel() const {
	VCard::AddressLabel addressLabel;
	addressLabel.isPreferred = getPreferred();
	addressLabel.isHome = getHome();
	addressLabel.isWork = getWork();
	addressLabel.deliveryType = domesticRadioButton->isChecked() ? VCard::DomesticDelivery : (internationalRadioButton->isChecked() ? VCard::InternationalDelivery : VCard::None);
	addressLabel.isPostal = getTagComboBox()->isTagSet("postal");
	addressLabel.isParcel = getTagComboBox()->isTagSet("parcel");

	std::string lines = Q2PSTRING(addressLabelPlainTextEdit->toPlainText());
	boost::split(addressLabel.lines, lines, boost::is_any_of("\n"));
	return addressLabel;
}

void QtVCardAddressLabelField::handleEditibleChanged(bool isEditable) {
	assert(addressLabelPlainTextEdit);
	assert(deliveryTypeLabel);
	assert(domesticRadioButton);
	assert(internationalRadioButton);

	addressLabelPlainTextEdit->setReadOnly(!isEditable);
	addressLabelPlainTextEdit->setStyleSheet(isEditable ? "" : "QPlainTextEdit { background: transparent; }");

	deliveryTypeLabel->setText(buttonGroup->checkedButton() == 0 ? "" : buttonGroup->checkedButton()->text());
	deliveryTypeLabel->setVisible(!isEditable);

	domesticRadioButton->setVisible(isEditable);
	internationalRadioButton->setVisible(isEditable);
}

}
