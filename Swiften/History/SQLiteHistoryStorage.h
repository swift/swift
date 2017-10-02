/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <thread>

#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/History/HistoryStorage.h>

struct sqlite3;

namespace Swift {
    class SWIFTEN_API SQLiteHistoryStorage : public HistoryStorage {
        public:
            SQLiteHistoryStorage(const boost::filesystem::path& file);
            ~SQLiteHistoryStorage();

            void addMessage(const HistoryMessage& message);
            ContactsMap getContacts(const JID& selfJID, HistoryMessage::Type type, const std::string& keyword) const;
            std::vector<HistoryMessage> getMessagesFromDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            std::vector<HistoryMessage> getMessagesFromNextDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            std::vector<HistoryMessage> getMessagesFromPreviousDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const;
            boost::posix_time::ptime getLastTimeStampFromMUC(const JID& selfJID, const JID& mucJID) const;

        private:
            void run();
            boost::gregorian::date getNextDateWithLogs(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date, bool reverseOrder) const;
            long long getIDForJID(const JID&);
            long long addJID(const JID&);

            boost::optional<JID> getJIDFromID(long long id) const;
            boost::optional<long long> getIDFromJID(const JID& jid) const;

            sqlite3* db_;
            std::thread* thread_;
    };
}
