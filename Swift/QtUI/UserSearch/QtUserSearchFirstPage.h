/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

#include <Swift/QtUI/UserSearch/QtSuggestingJIDInput.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchFirstPage.h>

namespace Swift {
    class UserSearchModel;
    class UserSearchDelegate;
    class UserSearchResult;
    class UIEventStream;

    class QtUserSearchFirstPage : public QWizardPage, public Ui::QtUserSearchFirstPage {
        Q_OBJECT
        public:
            QtUserSearchFirstPage(UserSearchWindow::Type type, const QString& title, SettingsProvider* settings);
            virtual bool isComplete() const;
        public slots:
            void emitCompletenessCheck();
        public:
            QtSuggestingJIDInput* jid_;
            QLabel* jidWarning_;
    };
}
