/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <boost/lexical_cast.hpp>

#include <sqlite3.h>
#include <Swiften/History/SQLiteHistoryManager.h>

namespace {

inline Swift::std::string getEscapedString(const Swift::std::string& s) {
	Swift::std::string result(s);
	result.replaceAll('\'', Swift::std::string("\\'"));
	return result;
}

}


namespace Swift {

SQLiteHistoryManager::SQLiteHistoryManager(const std::string& file) : db_(0) {
	sqlite3_open(file.c_str(), &db_);
	if (!db_) {
		std::cerr << "Error opening database " << file << std::endl; // FIXME
	}

	char* errorMessage;
	int result = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS messages('from' INTEGER, 'to' INTEGER, 'message' STRING, 'time' INTEGER)", 0, 0, &errorMessage);
	if (result != SQLITE_OK) {
		std::cerr << "SQL Error: " << errorMessage << std::endl;
		sqlite3_free(errorMessage);
	}

	result = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS jids('id' INTEGER PRIMARY KEY ASC AUTOINCREMENT, 'jid' STRING UNIQUE NOT NULL)", 0, 0, &errorMessage);
	if (result != SQLITE_OK) {
		std::cerr << "SQL Error: " << errorMessage << std::endl;
		sqlite3_free(errorMessage);
	}
}

SQLiteHistoryManager::~SQLiteHistoryManager() {
	sqlite3_close(db_);
}

void SQLiteHistoryManager::addMessage(const HistoryMessage& message) {
	int secondsSinceEpoch = (message.getTime() - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_seconds();
	std::string statement = std::string("INSERT INTO messages('from', 'to', 'message', 'time') VALUES(") + boost::lexical_cast<std::string>(getIDForJID(message.getFrom())) + ", " + boost::lexical_cast<std::string>(getIDForJID(message.getTo())) + ", '" + getEscapedString(message.getMessage()) + "', " + boost::lexical_cast<std::string>(secondsSinceEpoch) + ")";
	char* errorMessage;
	int result = sqlite3_exec(db_, statement.c_str(), 0, 0, &errorMessage);
	if (result != SQLITE_OK) {
		std::cerr << "SQL Error: " << errorMessage << std::endl;
		sqlite3_free(errorMessage);
	}
}

std::vector<HistoryMessage> SQLiteHistoryManager::getMessages() const {
	std::vector<HistoryMessage> result;
	sqlite3_stmt* selectStatement;
	std::string selectQuery("SELECT messages.'from', messages.'to', messages.'message', messages.'time' FROM messages");
	int r = sqlite3_prepare(db_, selectQuery.c_str(), selectQuery.size(), &selectStatement, NULL);
	if (r != SQLITE_OK) {
		std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
	}
	r = sqlite3_step(selectStatement);
	while (r == SQLITE_ROW) {
		boost::optional<JID> from(getJIDFromID(sqlite3_column_int(selectStatement, 0)));
		boost::optional<JID> to(getJIDFromID(sqlite3_column_int(selectStatement, 1)));
		std::string message(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 2)));
		int secondsSinceEpoch(sqlite3_column_int(selectStatement, 3));
		boost::posix_time::ptime time(boost::gregorian::date(1970, 1, 1), boost::posix_time::seconds(secondsSinceEpoch));

		result.push_back(HistoryMessage(message, (from ? *from : JID()), (to ? *to : JID()), time));
		r = sqlite3_step(selectStatement);
	}
	if (r != SQLITE_DONE) {
		std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
	}
	sqlite3_finalize(selectStatement);
	return result;
}

int SQLiteHistoryManager::getIDForJID(const JID& jid) {
	boost::optional<int> id = getIDFromJID(jid);
	if (id) {
		return *id;
	}
	else {
		return addJID(jid);
	}
}

int SQLiteHistoryManager::addJID(const JID& jid) {
	std::string statement = std::string("INSERT INTO jids('jid') VALUES('") + getEscapedString(jid.toString()) + "')";
	char* errorMessage;
	int result = sqlite3_exec(db_, statement.c_str(), 0, 0, &errorMessage);
	if (result != SQLITE_OK) {
		std::cerr << "SQL Error: " << errorMessage << std::endl;
		sqlite3_free(errorMessage);
	}
	return sqlite3_last_insert_rowid(db_);
}

boost::optional<JID> SQLiteHistoryManager::getJIDFromID(int id) const {
	boost::optional<JID> result;
	sqlite3_stmt* selectStatement;
	std::string selectQuery("SELECT jid FROM jids WHERE id=" + boost::lexical_cast<std::string>(id));
	int r = sqlite3_prepare(db_, selectQuery.c_str(), selectQuery.size(), &selectStatement, NULL);
	if (r != SQLITE_OK) {
		std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
	}
	r = sqlite3_step(selectStatement);
	if (r == SQLITE_ROW) {
		result = boost::optional<JID>(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 0)));
	}
	sqlite3_finalize(selectStatement);
	return result;
}

boost::optional<int> SQLiteHistoryManager::getIDFromJID(const JID& jid) const {
	boost::optional<int> result;
	sqlite3_stmt* selectStatement;
	std::string selectQuery("SELECT id FROM jids WHERE jid='" + jid.toString() + "'");
	int r = sqlite3_prepare(db_, selectQuery.c_str(), selectQuery.size(), &selectStatement, NULL);
	if (r != SQLITE_OK) {
		std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
	}
	r = sqlite3_step(selectStatement);
	if (r == SQLITE_ROW) {
		result = boost::optional<int>(sqlite3_column_int(selectStatement, 0));
	}
	sqlite3_finalize(selectStatement);
	return result;
}

}
