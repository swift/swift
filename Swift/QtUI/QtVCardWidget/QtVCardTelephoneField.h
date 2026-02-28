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

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtResizableLineEdit.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardHomeWork.h>

namespace Swift {

class QtVCardTelephoneField : public QtVCardGeneralField, public QtVCardHomeWork {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("Telephone"), UNLIMITED_INSTANCES, QtVCardTelephoneField)

        QtVCardTelephoneField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
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
