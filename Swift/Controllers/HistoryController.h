/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/signals2.hpp>

#include <Swiften/History/HistoryMessage.h>
#include <Swiften/History/HistoryStorage.h>

namespace Swift {
    class JID;

    class HistoryController {
        public:
            HistoryController(HistoryStorage* localHistoryStorage);
            ~HistoryController();

            void addMessage(const std::string& message, const JID& fromJID, const JID& toJID, HistoryMessage::Type type, const boost::posix_time::ptime& timeStamp);
            std::vector<HistoryMessage> getMessagesFromDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            std::vector<HistoryMessage> getMessagesFromPreviousDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            std::vector<HistoryMessage> getMessagesFromNextDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            ContactsMap getContacts(const JID& selfJID, HistoryMessage::Type type, const std::string& keyword = std::string()) const;
            std::vector<HistoryMessage> getMUCContext(const JID& selfJID, const JID& mucJID, const boost::posix_time::ptime& timeStamp) const;

            boost::posix_time::ptime getLastTimeStampFromMUC(const JID& selfJID, const JID& mucJID);

            boost::signals2::signal<void (const HistoryMessage&)> onNewMessage;

        private:
            HistoryStorage* localHistory_;
    };
}
