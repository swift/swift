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

class QtVCardJIDField : public QtVCardGeneralField {
    Q_OBJECT

    public:
        GENERIC_QT_VCARD_FIELD_INFO(tr("JID"), UNLIMITED_INSTANCES, QtVCardJIDField)

        QtVCardJIDField(QWidget* parent = nullptr, QGridLayout* layout = nullptr, bool editable = false);
        virtual ~QtVCardJIDField();

        virtual bool isEmpty() const;

        void setJID(const JID& jid);
        JID getJID() const;

    protected:
        virtual void setupContentWidgets();

    public slots:
        void handleEditibleChanged(bool isEditable);

    private:
        QLabel* jidLabel;
        QtResizableLineEdit* jidLineEdit;
};

}
