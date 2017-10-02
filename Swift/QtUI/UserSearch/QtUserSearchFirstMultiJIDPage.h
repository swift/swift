/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QWizardPage>

#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchFirstMultiJIDPage.h>

namespace Swift {
    class UserSearchModel;
    class UserSearchDelegate;
    class UserSearchResult;
    class UIEventStream;
    class QtContactListWidget;
    class ContactSuggester;
    class AvatarManager;
    class VCardManager;
    class SettingsProvider;
    class QtSuggestingJIDInput;

    class QtUserSearchFirstMultiJIDPage : public QWizardPage, public Ui::QtUserSearchFirstMultiJIDPage {
        Q_OBJECT
        public:
            QtUserSearchFirstMultiJIDPage(UserSearchWindow::Type type, const QString& title, SettingsProvider* settings);
            virtual bool isComplete() const;
            void reset();

        signals:
            void onJIDsDropped(std::vector<JID> jid);

        public slots:
            void emitCompletenessCheck();
            void handleEditingDone();
            virtual void dragEnterEvent(QDragEnterEvent *event);
            virtual void dropEvent(QDropEvent *event);

        public:
            QtContactListWidget* contactList_;
            QtSuggestingJIDInput* jid_;
    };
}
