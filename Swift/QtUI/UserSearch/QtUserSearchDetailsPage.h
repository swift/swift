/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>
#include <string>
#include <vector>

#include <QWizardPage>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchFieldsPage.h>

namespace Swift {
    class QtContactEditWidget;

    class QtUserSearchDetailsPage : public QWizardPage {
            Q_OBJECT
        public:
            QtUserSearchDetailsPage(const std::set<std::string>& availableGroups);
            virtual ~QtUserSearchDetailsPage();

            void setJID(const JID& jid);
            void setNameSuggestions(const std::vector<std::string>& nameSuggestions);
            void setName(const std::string& name);

            std::set<std::string> getSelectedGroups();
            std::string getName();

            void clear();

        signals:
            void onUserTriggersFinish();

        private:
            QtContactEditWidget* editWidget;
            JID contactJID;
    };
}
