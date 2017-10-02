/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchResultsPage.h>

namespace Swift {
    class QtUserSearchResultsPage : public QWizardPage, public Ui::QtUserSearchResultsPage {
        Q_OBJECT
        public:
            QtUserSearchResultsPage();
            virtual bool isComplete() const;
            void setNoResults(bool noResults);
        signals:
          void onUserTriggersContinue();
        public slots:
            void emitCompletenessCheck();
    };
}
