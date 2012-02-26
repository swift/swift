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

class QtVCardTitleField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Title", UNLIMITED_INSTANCES, QtVCardTitleField)

		QtVCardTitleField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardTitleField();

		virtual bool isEmpty() const;

		void setTitle(const std::string& title);
		std::string getTitle() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QtResizableLineEdit* titleLineEdit;
};

}
