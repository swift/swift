/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QTreeWidget>

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtRemovableItemDelegate.h>
#include <Swift/QtUI/QtVCardWidget/QtResizableLineEdit.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>

namespace Swift {

class QtVCardOrganizationField : public QtVCardGeneralField {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("Organization"), UNLIMITED_INSTANCES, QtVCardOrganizationField)

        QtVCardOrganizationField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
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
        void handleRowsRemoved(const QModelIndex&, int, int);

    private:
        void guaranteeEmptyRow();

    private:
        QLabel* organizationLabel;
        QtResizableLineEdit* organizationLineEdit;
        QTreeWidget* unitsTreeWidget;
        QtRemovableItemDelegate* itemDelegate;
};

}
