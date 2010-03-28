#pragma once

#include <boost/optional.hpp>

#include "Swiften/History/HistoryManager.h"

struct sqlite3;

namespace Swift {
	class SQLiteHistoryManager : public HistoryManager {
		public:
			SQLiteHistoryManager(const String& file);
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
