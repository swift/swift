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
	class UIEventStream;
	class QtMUCSearchWindow : public QWidget, public MUCSearchWindow, private Ui::QtMUCSearchWindow {
		Q_OBJECT
		public:
			QtMUCSearchWindow(UIEventStream* eventStream);
			virtual ~QtMUCSearchWindow();

			virtual void setNick(const String& nick);
			virtual void setMUC(const String& nick);
			virtual void clearList();
			virtual void addService(const MUCService& service);
			virtual void addSavedServices(const std::vector<JID>& services);
			virtual void setSearchInProgress(bool searching);

			virtual void show();
		protected:
			virtual void resizeEvent(QResizeEvent* event);
		private slots:
			void handleSearch(const QString& text);
			void handleSearch();
			void handleJoin();
			void handleSelected(const QModelIndex& current);
			void handleActivated(const QModelIndex& index);
			void updateThrobberPosition();
		private:
			void createAutoJoin(const JID& room, boost::optional<String> passedNick);
			MUCSearchModel* model_;
			MUCSearchDelegate* delegate_;
			UIEventStream* eventStream_;
			QLabel* throbber_;
			String lastSetNick_;
			bool hasHadScrollBars_;
	};
}
