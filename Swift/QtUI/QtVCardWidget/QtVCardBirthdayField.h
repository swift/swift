/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QDateEdit>
#include <Swiften/Elements/VCard.h>

#include "QtCloseButton.h"
#include "QtVCardFieldInfo.h"
#include "QtVCardGeneralField.h"

namespace Swift {

class QtVCardBirthdayField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Birthday", 1, QtVCardBirthdayField)

		QtVCardBirthdayField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardBirthdayField();

		virtual bool isEmpty() const;

		void setBirthday(const boost::posix_time::ptime& addressLabel);
		boost::posix_time::ptime getBirthday() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QLabel* birthdayLabel;
		QDateEdit* birthdayDateEdit;
};

}
