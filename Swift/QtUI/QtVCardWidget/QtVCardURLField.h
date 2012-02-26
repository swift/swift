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

class QtVCardURLField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("URL", UNLIMITED_INSTANCES, QtVCardURLField)

		QtVCardURLField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardURLField();

		virtual bool isEmpty() const;

		void setURL(const std::string& url);
		std::string getURL() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private:
		QLabel* urlLabel;
		QtResizableLineEdit* urlLineEdit;
};

}
