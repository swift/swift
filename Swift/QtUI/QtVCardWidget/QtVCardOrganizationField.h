/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>

#include <QTreeWidget>

#include "QtRemovableItemDelegate.h"
#include "QtResizableLineEdit.h"
#include "QtVCardFieldInfo.h"
#include "QtVCardGeneralField.h"

namespace Swift {

class QtVCardOrganizationField : public QtVCardGeneralField {
	Q_OBJECT

	public:
		GENERIC_QT_VCARD_FIELD_INFO("Organization", UNLIMITED_INSTANCES, QtVCardOrganizationField)

		QtVCardOrganizationField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false);
		virtual ~QtVCardOrganizationField();

		virtual bool isEmpty() const;

		void setOrganization(const VCard::Organization& organization);
		VCard::Organization getOrganization() const;

	protected:
		virtual void setupContentWidgets();

	public slots:
		void handleEditibleChanged(bool isEditable);

	private slots:
		void handleItemChanged(QTreeWidgetItem*, int);

	private:
		QLabel* organizationLabel;
		QtResizableLineEdit* organizationLineEdit;
		QTreeWidget* unitsTreeWidget;
		QtRemovableItemDelegate* itemDelegate;
};

}
