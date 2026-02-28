/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <QWidget>

#include <Swiften/Elements/VCard.h>
#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIInterfaces/ContactEditWindow.h>

class QLabel;
class QVBoxLayout;

namespace Swift {
    class QtContactEditWidget;

    class QtContactEditWindow : public QWidget, public ContactEditWindow {
            Q_OBJECT

        public:
            QtContactEditWindow();
            virtual ~QtContactEditWindow();

            virtual void setNameSuggestions(const std::vector<std::string>& nameSuggestions);
            virtual void setContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, const std::set<std::string>& allGroups);

            void setEnabled(bool);
            void show();
            void hide();

            static bool confirmContactDeletion(const JID& jid);

        private slots:
            void handleRemoveContact();
            void handleUpdateContact();

        private:
            JID jid_;
            QVBoxLayout* groupsLayout_;
            QLabel* jidLabel_;
            QtContactEditWidget* contactEditWidget_;
    };
}

