/*
 * Copyright (c) 2010-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizard>
#include <set>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchWizard.h>
#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>

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

			virtual void addSavedServices(const std::vector<JID>& services);

			virtual void clear();
			virtual void show();
			virtual void setResults(const std::vector<UserSearchResult>& results);
			virtual void setResultsForm(Form::ref results);
			virtual void setSelectedService(const JID& jid);
			virtual void setServerSupportsSearch(bool error);
			virtual void setSearchError(bool error);
			virtual void setSearchFields(boost::shared_ptr<SearchPayload> fields);
			virtual void setNameSuggestions(const std::vector<std::string>& suggestions);
			virtual void prepopulateJIDAndName(const JID& jid, const std::string& name);
			virtual void setContactSuggestions(const std::vector<Contact>& suggestions);
			virtual void setJIDs(const std::vector<JID> &jids);
			virtual void setRoomJID(const JID &roomJID);
			virtual std::string getReason() const;
			virtual std::vector<JID> getJIDs() const;
			virtual void setCanStartImpromptuChats(bool supportsImpromptu);
			virtual void updateContacts(const std::vector<Contact> &contacts);
			virtual void addContacts(const std::vector<Contact>& contacts);

		protected:
			virtual int nextId() const;

		private slots:
			void handleFirstPageRadioChange();
			virtual void handleCurrentChanged(int);
			virtual void handleAccepted();
			void handleContactSuggestionRequested(const QString& text);
			void addContact();
			void handleAddViaSearch();
			void handleListChanged(std::vector<Contact> list);
			void handleJIDsAdded(std::vector<JID> jids);

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
			void addSearchedJIDToList(const JID& jid);

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
			int lastPage_;
			std::vector<Contact> contactVector_;
			SettingsProvider* settings_;
			bool searchNext_;
			bool supportsImpromptu_;
	};
}
