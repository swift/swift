/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <QWizard>

#include <Swiften/Base/Override.h>

#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchWizard.h>

namespace Swift {
    class UserSearchModel;
    class UserSearchDelegate;
    class UserSearchResult;
    class UIEventStream;
    class QtUserSearchFirstPage;
    class QtUserSearchFirstMultiJIDPage;
    class QtUserSearchFieldsPage;
    class QtUserSearchResultsPage;
    class QtUserSearchDetailsPage;
    class QtFormResultItemModel;
    class SettingsProvider;

    class QtUserSearchWindow : public QWizard, public UserSearchWindow, private Ui::QtUserSearchWizard {
        Q_OBJECT
        public:
            QtUserSearchWindow(UIEventStream* eventStream, UserSearchWindow::Type type, const std::set<std::string>& groups, SettingsProvider* settingsProvider);
            virtual ~QtUserSearchWindow();

            virtual void addSavedServices(const std::vector<JID>& services) SWIFTEN_OVERRIDE;

            virtual void clear() SWIFTEN_OVERRIDE;
            virtual void show() SWIFTEN_OVERRIDE;
            virtual void setResults(const std::vector<UserSearchResult>& results) SWIFTEN_OVERRIDE;
            virtual void setResultsForm(Form::ref results) SWIFTEN_OVERRIDE;
            virtual void setSelectedService(const JID& jid) SWIFTEN_OVERRIDE;
            virtual void setServerSupportsSearch(bool error) SWIFTEN_OVERRIDE;
            virtual void setSearchError(bool error) SWIFTEN_OVERRIDE;
            virtual void setSearchFields(std::shared_ptr<SearchPayload> fields) SWIFTEN_OVERRIDE;
            virtual void setNameSuggestions(const std::vector<std::string>& suggestions) SWIFTEN_OVERRIDE;
            virtual void prepopulateJIDAndName(const JID& jid, const std::string& name) SWIFTEN_OVERRIDE;
            virtual void setContactSuggestions(const std::vector<Contact::ref>& suggestions) SWIFTEN_OVERRIDE;
            virtual void setJIDs(const std::vector<JID> &jids) SWIFTEN_OVERRIDE;
            virtual void setOriginator(const JID& originator) SWIFTEN_OVERRIDE;
            virtual void setRoomJID(const JID &roomJID) SWIFTEN_OVERRIDE;
            virtual std::string getReason() const SWIFTEN_OVERRIDE;
            virtual std::vector<JID> getJIDs() const SWIFTEN_OVERRIDE;
            virtual void setCanStartImpromptuChats(bool supportsImpromptu) SWIFTEN_OVERRIDE;
            virtual void updateContacts(const std::vector<Contact::ref> &contacts) SWIFTEN_OVERRIDE;
            virtual void addContacts(const std::vector<Contact::ref>& contacts) SWIFTEN_OVERRIDE;
            virtual void setCanSupplyDescription(bool allowed) SWIFTEN_OVERRIDE;
            virtual void setWarning(const boost::optional<std::string>& message) SWIFTEN_OVERRIDE;

        protected:
            virtual int nextId() const SWIFTEN_OVERRIDE;

        private slots:
            void handleFirstPageRadioChange();
            void handleCurrentChanged(int);
            void handleAccepted();
            void handleContactSuggestionRequested(const QString& text);
            void addContact();
            void handleAddViaSearch();
            void handleListChanged(std::vector<Contact::ref> list);
            void handleJIDsAdded(std::vector<JID> jids);
            void handleJIDEditingDone();

        private:
            void setFirstPage(QString title = "");
            void setSecondPage();
            void setThirdPage();

        private:
            void clearForm();
            void setError(const QString& error);
            JID getServerToSearch();
            void handleSearch();
            JID getContactJID() const;
            Contact::ref getContact() const;
            void addSearchedJIDToList(const Contact::ref& contact);
            void handleOnSearchedJIDSelected(const Contact::ref& contact);

        private:
            UIEventStream* eventStream_;
            UserSearchWindow::Type type_;
            QAbstractItemModel* model_;
            UserSearchDelegate* delegate_;
            QtUserSearchFirstPage* firstPage_;
            QtUserSearchFirstMultiJIDPage* firstMultiJIDPage_;
            QtUserSearchFieldsPage* fieldsPage_;
            QtUserSearchResultsPage* resultsPage_;
            QtUserSearchDetailsPage* detailsPage_;
            JID myServer_;
            JID roomJID_;
            JID originatorJID_;
            int lastPage_;
            std::vector<Contact::ref> contactVector_;
            SettingsProvider* settings_;
            bool searchNext_;
            bool supportsImpromptu_;
    };
}
