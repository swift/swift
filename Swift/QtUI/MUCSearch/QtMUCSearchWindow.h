/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/QtUI/MUCSearch/ui_QtMUCSearchWindow.h"

#include "Swift/Controllers/UIInterfaces/MUCSearchWindow.h"

namespace Swift {
	class MUCSearchModel;
	class MUCSearchDelegate;
	class MUCSearchRoomItem;

	class QtMUCSearchWindow : public QDialog, public MUCSearchWindow {
		Q_OBJECT
		public:
			QtMUCSearchWindow();
			virtual ~QtMUCSearchWindow();

			virtual void clearList();
			virtual void addService(const MUCService& service);
			virtual void addSavedServices(const std::list<JID>& services);
			virtual void setSearchInProgress(bool searching);

			virtual void show();
			virtual void accept();
			virtual void reject();

		protected:
			virtual void resizeEvent(QResizeEvent* event);

		private slots:
			void handleSearch();
			void handleSearch(const QString&);
			void handleActivated(const QModelIndex& index);
			void updateThrobberPosition();
			void handleSelectionChanged (const QItemSelection&, const QItemSelection&);
			MUCSearchRoomItem* getSelectedRoom() const;

		private:
			Ui::QtMUCSearchWindow ui_;
			MUCSearchModel* model_;
			MUCSearchDelegate* delegate_;
			QLabel* throbber_;
			bool hasHadScrollBars_;
	};
}
