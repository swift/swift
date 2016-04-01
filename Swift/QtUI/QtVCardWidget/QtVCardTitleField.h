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

class QtVCardTitleField : public QtVCardGeneralField {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("Title"), UNLIMITED_INSTANCES, QtVCardTitleField)

        QtVCardTitleField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
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
