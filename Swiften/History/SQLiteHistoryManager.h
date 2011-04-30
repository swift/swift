/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/History/HistoryManager.h>

struct sqlite3;

namespace Swift {
	class SQLiteHistoryManager : public HistoryManager {
		public:
			SQLiteHistoryManager(const std::string& file);
			~SQLiteHistoryManager();

			virtual void addMessage(const HistoryMessage& message);
			virtual std::vector<HistoryMessage> getMessages() const;

			int getIDForJID(const JID&);
			int addJID(const JID&);

			boost::optional<JID> getJIDFromID(int id) const;
			boost::optional<int> getIDFromJID(const JID& jid) const;

		private:
			sqlite3* db_;
	};
}
