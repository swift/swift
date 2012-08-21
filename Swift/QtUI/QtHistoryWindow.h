/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swift/Controllers/UIInterfaces/HistoryWindow.h>
#include <Swift/QtUI/ui_QtHistoryWindow.h>
#include <QtChatView.h>
#include <QtTabbable.h>
#include <Swift/QtUI/Roster/QtTreeWidget.h>
#include <set>
#include <QDate>

namespace Swift {
	class QtHistoryWindow : public QtTabbable, public HistoryWindow {
			Q_OBJECT

		public:
			QtHistoryWindow(SettingsProvider*, UIEventStream*);
			~QtHistoryWindow();
			void activate();
			void setRosterModel(Roster*);
			void addMessage(const std::string& message, const std::string& senderName, bool senderIsSelf, const std::string& avatarPath, const boost::posix_time::ptime& time, bool addAtTheTop);
			void resetConversationView();
			void resetConversationViewTopInsertPoint();
			void setDate(const boost::gregorian::date& date);

			void closeEvent(QCloseEvent* event);
			void showEvent(QShowEvent* event);

			std::string getSearchBoxText();
			boost::gregorian::date getLastVisibleDate();

		signals:
			void fontResized(int);

		public slots:
			void handleFontResized(int fontSizeSteps);

		protected slots:
			void handleScrollRequested(int pos);
			void handleScrollReachedTop();
			void handleScrollReachedBottom();
			void handleReturnPressed();
			void handleCalendarClicked(const QDate& date);
			void handlePreviousButtonClicked();
			void handleNextButtonClicked();

		private:
			void handleSomethingSelectedChanged(RosterItem* item);

			Ui::QtHistoryWindow ui_;
			QtChatTheme* theme_;
			QtChatView* conversation_;
			QtTreeWidget* conversationRoster_;
			std::set<QDate> dates_;
			int idCounter_;
			bool previousTopMessageWasSelf_;
			QString previousTopSenderName_;
			bool previousBottomMessageWasSelf_;
			QString previousBottomSenderName_;
	};
}
