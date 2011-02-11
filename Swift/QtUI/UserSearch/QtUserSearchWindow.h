/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWizard>

#include <Swift/QtUI/UserSearch/ui_QtUserSearchWizard.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchFirstPage.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchFieldsPage.h>
#include <Swift/QtUI/UserSearch/ui_QtUserSearchResultsPage.h>

#include <Swift/Controllers/UIInterfaces/UserSearchWindow.h>


namespace Swift {
	class UserSearchModel;
	class UserSearchDelegate;
	class UserSearchResult;
	class UIEventStream;

	class QtUserSearchFirstPage : public QWizardPage, public Ui::QtUserSearchFirstPage {
		Q_OBJECT
		public:
			QtUserSearchFirstPage(UserSearchWindow::Type type, const QString& title);
			virtual bool isComplete() const;
		public slots:
			void emitCompletenessCheck();
	};

	class QtUserSearchFieldsPage : public QWizardPage, public Ui::QtUserSearchFieldsPage {
		Q_OBJECT
		public:
			QtUserSearchFieldsPage();
			virtual bool isComplete() const;
		public slots:
			void emitCompletenessCheck();
	};

	class QtUserSearchResultsPage : public QWizardPage, public Ui::QtUserSearchResultsPage {
		Q_OBJECT
		public:
			QtUserSearchResultsPage();
			virtual bool isComplete() const;
		signals:
		  void onUserTriggersFinish();
		public slots:
			void emitCompletenessCheck();
	};

	class QtUserSearchWindow : public QWizard, public UserSearchWindow, private Ui::QtUserSearchWizard {
		Q_OBJECT
		public:
			QtUserSearchWindow(UIEventStream* eventStream, UserSearchWindow::Type type);
			virtual ~QtUserSearchWindow();

			virtual void addSavedServices(const std::vector<JID>& services);

			virtual void clear();
			virtual void show();
			virtual void setResults(const std::vector<UserSearchResult>& results);
			virtual void setSelectedService(const JID& jid);
			virtual void setServerSupportsSearch(bool error);
			virtual void setSearchError(bool error);
			virtual void setSearchFields(boost::shared_ptr<SearchPayload> fields) ;
		protected:
			virtual int nextId() const;
		private slots:
			void handleFirstPageRadioChange();
			virtual void handleCurrentChanged(int);
			virtual void handleAccepted();
		private:
			void clearForm();
			void setError(const QString& error);
			JID searchServer();
			void handleSearch();
			UserSearchWindow::Type type_;
			UserSearchModel* model_;
			UserSearchDelegate* delegate_;
			QtUserSearchFirstPage* firstPage_;
			QtUserSearchFieldsPage* fieldsPage_;
			QtUserSearchResultsPage* resultsPage_;
			UIEventStream* eventStream_;
			JID myServer_;
			int lastPage_;
	};
}
