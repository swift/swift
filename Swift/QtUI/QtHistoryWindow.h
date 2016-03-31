/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <QDate>

#include <Swift/Controllers/UIInterfaces/HistoryWindow.h>

#include <Swift/QtUI/QtTabbable.h>
#include <Swift/QtUI/ui_QtHistoryWindow.h>

namespace Swift {
    class QtTabbable;
    class QtTreeWidget;
    class QtWebKitChatView;
    class QtChatTheme;
    class SettingsProvider;
    class UIEventStream;

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

            virtual std::string getID() const;

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
            QtWebKitChatView* conversation_;
            QtTreeWidget* conversationRoster_;
            std::set<QDate> dates_;
            int idCounter_;
            bool previousTopMessageWasSelf_;
            QString previousTopSenderName_;
            bool previousBottomMessageWasSelf_;
            QString previousBottomSenderName_;
    };
}
