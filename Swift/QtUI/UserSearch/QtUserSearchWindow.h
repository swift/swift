/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <QWizard>

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
            virtual ~QtUserSearchWindow() override;

            virtual void addSavedServices(const std::vector<JID>& services) override;

            virtual void clear() override;
            virtual void show() override;
            virtual void setResults(const std::vector<UserSearchResult>& results) override;
            virtual void setResultsForm(Form::ref results) override;
            virtual void setSelectedService(const JID& jid) override;
            virtual void setServerSupportsSearch(bool error) override;
            virtual void setSearchError(bool error) override;
            virtual void setSearchFields(std::shared_ptr<SearchPayload> fields) override;
            virtual void setNameSuggestions(const std::vector<std::string>& suggestions) override;
            virtual void prepopulateJIDAndName(const JID& jid, const std::string& name) override;
            virtual void setContactSuggestions(const std::vector<Contact::ref>& suggestions) override;
            virtual void setJIDs(const std::vector<JID> &jids) override;
            virtual void setOriginator(const JID& originator) override;
            virtual void setRoomJID(const JID &roomJID) override;
            virtual std::string getReason() const override;
            virtual std::vector<JID> getJIDs() const override;
            virtual void setCanStartImpromptuChats(bool supportsImpromptu) override;
            virtual void updateContacts(const std::vector<Contact::ref> &contacts) override;
            virtual void addContacts(const std::vector<Contact::ref>& contacts) override;
            virtual void setCanSupplyDescription(bool allowed) override;
            virtual void setWarning(const boost::optional<std::string>& message) override;

        protected:
            virtual int nextId() const override;

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
