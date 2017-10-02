/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Roster/Roster.h>

namespace Swift {
    class HistoryWindow {
        public:
            virtual ~HistoryWindow() {}

            virtual void activate() = 0;
            virtual void setRosterModel(Roster*) = 0;
            virtual void addMessage(const std::string &message, const std::string &senderName, bool senderIsSelf, const std::string& avatarPath, const boost::posix_time::ptime& time, bool addAtTheTop) = 0;
            virtual void resetConversationView() = 0;
            virtual void resetConversationViewTopInsertPoint() = 0; // this is a temporary fix used in adding messages at the top
            virtual void setDate(const boost::gregorian::date& date) = 0;

            virtual std::string getSearchBoxText() = 0;
            virtual boost::gregorian::date getLastVisibleDate() = 0;

            boost::signals2::signal<void (RosterItem*)> onSelectedContactChanged;
            boost::signals2::signal<void (const std::string&)> onReturnPressed;
            boost::signals2::signal<void (const boost::gregorian::date&)> onScrollReachedTop;
            boost::signals2::signal<void (const boost::gregorian::date&)> onScrollReachedBottom;
            boost::signals2::signal<void ()> onPreviousButtonClicked;
            boost::signals2::signal<void ()> onNextButtonClicked;
            boost::signals2::signal<void (const boost::gregorian::date&)> onCalendarClicked;
    };
}
