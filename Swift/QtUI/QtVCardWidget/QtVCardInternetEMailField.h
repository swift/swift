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

class QtVCardInternetEMailField : public QtVCardGeneralField, public QtVCardHomeWork {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("E-Mail"), UNLIMITED_INSTANCES, QtVCardInternetEMailField)

        QtVCardInternetEMailField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
        virtual ~QtVCardInternetEMailField();

        virtual bool isEmpty() const;

        void setInternetEMailAddress(const VCard::EMailAddress& address);
        VCard::EMailAddress getInternetEMailAddress() const;

    protected:
        virtual void setupContentWidgets();

    public slots:
        void handleEditibleChanged(bool isEditable);

    private:
        QtResizableLineEdit* emailLineEdit;
        QLabel* emailLabel;
};

}
