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

#include <QDateEdit>

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtCloseButton.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>

namespace Swift {

class QtVCardBirthdayField : public QtVCardGeneralField {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("Birthday"), 1, QtVCardBirthdayField)

        QtVCardBirthdayField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
        virtual ~QtVCardBirthdayField();

        virtual bool isEmpty() const;

        void setBirthday(const boost::posix_time::ptime& addressLabel);
        boost::posix_time::ptime getBirthday() const;

    protected:
        virtual void setupContentWidgets();

    public slots:
        void handleEditibleChanged(bool isEditable);

    private:
        QLabel* birthdayLabel;
        QDateEdit* birthdayDateEdit;
};

}
