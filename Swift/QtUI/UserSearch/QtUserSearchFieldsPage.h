/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/QtUI/QtFormWidget.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchFieldsPage.h>

namespace Swift {
    class QtUserSearchFieldsPage : public QWizardPage, public Ui::QtUserSearchFieldsPage {
        Q_OBJECT
        public:
            QtUserSearchFieldsPage();
            virtual bool isComplete() const;

            QtFormWidget* getFormWidget();
            void setFormWidget(QtFormWidget *widget);

        public slots:
            void emitCompletenessCheck();

        private:
            QtFormWidget *formWidget_;
    };
}
