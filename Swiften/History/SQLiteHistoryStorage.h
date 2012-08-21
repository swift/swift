/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/History/HistoryStorage.h>
#include <boost/thread.hpp>

struct sqlite3;

namespace Swift {
	class SQLiteHistoryStorage : public HistoryStorage {
		public:
			SQLiteHistoryStorage(const std::string& file);
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
			int getIDForJID(const JID&);
			int addJID(const JID&);

			boost::optional<JID> getJIDFromID(int id) const;
			boost::optional<int> getIDFromJID(const JID& jid) const;

			sqlite3* db_;
			boost::thread* thread_;
	};
}
