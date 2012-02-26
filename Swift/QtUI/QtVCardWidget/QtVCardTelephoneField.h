/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>

#include "QtResizableLineEdit.h"
#include "QtVCardFieldInfo.h"
#include "QtVCardGeneralField.h"
#include "QtVCardHomeWork.h"

namespace Swift {

class QtVCardTelephoneField : public QtVCardGeneralField, public QtVCardHomeWork {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Telephone", UNLIMITED_INSTANCES, QtVCardTelephoneField)

		QtVCardTelephoneField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardTelephoneField();

		virtual bool isEmpty() const;

		void setTelephone(const VCard::Telephone& telephone);
		VCard::Telephone getTelephone() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QtResizableLineEdit* telephoneLineEdit;
};

}
