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

namespace Swift {

class QtVCardRoleField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Role", UNLIMITED_INSTANCES, QtVCardRoleField)

		QtVCardRoleField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardRoleField();

		virtual bool isEmpty() const;

		void setRole(const std::string& role);
		std::string getRole() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QtResizableLineEdit* roleLineEdit;
};

}
