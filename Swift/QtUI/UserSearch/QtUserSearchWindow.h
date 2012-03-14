/*
 * Copyright (c) 2010 Kevin Smith
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
	class QtUserSearchFieldsPage;
	class QtUserSearchResultsPage;
	class QtUserSearchDetailsPage;
	class QtFormResultItemModel;

	class QtUserSearchWindow : public QWizard, public UserSearchWindow, private Ui::QtUserSearchWizard {
		Q_OBJECT
		public:
			QtUserSearchWindow(UIEventStream* eventStream, UserSearchWindow::Type type, const std::set<std::string>& groups);
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

		protected:
			virtual int nextId() const;
		private slots:
			void handleFirstPageRadioChange();
			virtual void handleCurrentChanged(int);
			virtual void handleAccepted();
		private:
			void clearForm();
			void setError(const QString& error);
			JID getServerToSearch();
			void handleSearch();
			JID getContactJID() const;

		private:
			UIEventStream* eventStream_;
			UserSearchWindow::Type type_;
			QAbstractItemModel* model_;
			UserSearchDelegate* delegate_;
			QtUserSearchFirstPage* firstPage_;
			QtUserSearchFieldsPage* fieldsPage_;
			QtUserSearchResultsPage* resultsPage_;
			QtUserSearchDetailsPage* detailsPage_;
			JID myServer_;
			int lastPage_;
	};
}
