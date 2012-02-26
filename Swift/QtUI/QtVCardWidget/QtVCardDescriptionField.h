/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>

#include <QPlainTextEdit>

#include "QtVCardFieldInfo.h"
#include "QtVCardGeneralField.h"

namespace Swift {

class QtVCardDescriptionField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Description", 1, QtVCardDescriptionField)

		QtVCardDescriptionField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardDescriptionField();

		virtual bool isEmpty() const;

		void setDescription(const std::string& description);
		std::string getDescription() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QPlainTextEdit* descriptionPlainTextEdit;
};

}
