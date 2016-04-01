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

namespace Swift {

class QtVCardRoleField : public QtVCardGeneralField {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("Role"), UNLIMITED_INSTANCES, QtVCardRoleField)

        QtVCardRoleField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
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
