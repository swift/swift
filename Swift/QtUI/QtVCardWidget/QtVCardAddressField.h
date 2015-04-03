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
#include <QRadioButton>

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtResizableLineEdit.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardHomeWork.h>

namespace Swift {

class QtElidingLabel;

class QtVCardAddressField : public QtVCardGeneralField, public QtVCardHomeWork {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Address", UNLIMITED_INSTANCES, QtVCardAddressField)

		QtVCardAddressField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardAddressField();

		virtual bool isEmpty() const;

		void setAddress(const VCard::Address& address);
		VCard::Address getAddress() const;

	protected:
		virtual void setupContentWidgets();
		virtual void customCleanup();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QList<QWidget*> textFields;
		QtResizableLineEdit* streetLineEdit;
		QtResizableLineEdit* poboxLineEdit;
		QtResizableLineEdit* addressextLineEdit;
		QtResizableLineEdit* cityLineEdit;
		QtResizableLineEdit* pocodeLineEdit;
		QtResizableLineEdit* regionLineEdit;
		QtResizableLineEdit* countryLineEdit;
		QGridLayout* textFieldGridLayout;
		QLayoutItem* textFieldGridLayoutItem;

		QtElidingLabel* deliveryTypeLabel;
		QRadioButton* domesticRadioButton;
		QRadioButton* internationalRadioButton;
		QButtonGroup* buttonGroup;
};

}
