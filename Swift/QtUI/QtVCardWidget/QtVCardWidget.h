/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <list>
#include <memory>

#include <QToolButton>
#include <QWidget>

#include <Swiften/Elements/VCard.h>

#include <Swift/QtUI/QtVCardWidget/QtVCardFieldInfo.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardGeneralField.h>
#include <Swift/QtUI/QtVCardWidget/QtVCardPhotoAndNameFields.h>

namespace Ui {
    class QtVCardWidget;
}

namespace Swift {

    class QtVCardWidget : public QWidget {
        Q_OBJECT
        Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

        public :
            explicit QtVCardWidget(QWidget* parent = nullptr);
            ~QtVCardWidget();

            bool isEditable() const;
            void setEditable(bool);

            void setVCard(VCard::ref vcard);
            VCard::ref getVCard();

            virtual QSize sizeHint() const;

        signals:
            void editableChanged(bool editable);

        private slots:
            void addField();
            void removeField(QtVCardGeneralField* field);

        private:
            void addFieldType(QMenu*, std::shared_ptr<QtVCardFieldInfo>);
            int fieldTypeInstances(std::shared_ptr<QtVCardFieldInfo>);
            void clearFields();
            void clearEmptyFields();
            void appendField(QtVCardGeneralField* field);
            void relayoutToolButton();

        private:
            VCard::ref vcard;
            Ui::QtVCardWidget* ui;
            QToolButton* toolButton;
            bool editable;
            QMenu* menu;
            std::list<QtVCardGeneralField*> fields;
            std::map<QAction*, std::shared_ptr<QtVCardFieldInfo> > actionFieldInfo;
    };

}
