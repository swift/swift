/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QRadioButton>

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardHomeWork.h>

namespace Swift {

class QtElidingLabel;

class QtVCardAddressLabelField : public QtVCardGeneralField, public QtVCardHomeWork {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Address Label", UNLIMITED_INSTANCES, QtVCardAddressLabelField)

		QtVCardAddressLabelField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardAddressLabelField();

		virtual bool isEmpty() const;

		void setAddressLabel(const VCard::AddressLabel& addressLabel);
		VCard::AddressLabel getAddressLabel() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QPlainTextEdit* addressLabelPlainTextEdit;

		QtElidingLabel* deliveryTypeLabel;
		QRadioButton* domesticRadioButton;
		QRadioButton* internationalRadioButton;
		QButtonGroup* buttonGroup;
};

}
