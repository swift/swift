/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/UserSearch/ui_QtUserSearchWindow.h"

#include "Swift/Controllers/UIInterfaces/UserSearchWindow.h"


namespace Swift {
	class UserSearchModel;
	class UserSearchDelegate;
	class UserSearchResult;
	class UIEventStream;
	class QtUserSearchWindow : public QWidget, public UserSearchWindow, private Ui::QtUserSearchWindow {
		Q_OBJECT
		public:
			QtUserSearchWindow(UIEventStream* eventStream);
			virtual ~QtUserSearchWindow();

			virtual void addSavedServices(const std::vector<JID>& services);

			virtual void clear();
			virtual void show();
			virtual void setResults(const std::vector<UserSearchResult>& results);
			virtual void setSelectedService(const JID& jid);
			virtual void setServerSupportsSearch(bool error);
			virtual void setSearchError(bool error);
			virtual void setSearchFields(boost::shared_ptr<SearchPayload> fields) ;
		private slots:
			void handleGetForm();
			void handleSelected(const QModelIndex& current);
			void handleSearch();
			void handleActivated(const QModelIndex& index);
			void handleOkClicked();
			void handleCancelClicked();
			void enableCorrectButtons();
		private:
			UserSearchModel* model_;
			UserSearchDelegate* delegate_;
			UIEventStream* eventStream_;
			JID lastServiceJID_;
	};
}
