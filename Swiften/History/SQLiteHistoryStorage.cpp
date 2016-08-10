/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/History/SQLiteHistoryStorage.h>

#include <iostream>

#include <boost/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <sqlite3.h>

#include <Swiften/Base/Path.h>

inline std::string getEscapedString(const std::string& s) {
    std::string result(s);

    size_t pos = result.find('\'');
    while (pos != std::string::npos) {
        result.insert(pos, "'");
        pos = result.find('\'', pos + 2);
    }
    return result;
}

namespace Swift {

SQLiteHistoryStorage::SQLiteHistoryStorage(const boost::filesystem::path& file) : db_(nullptr) {
    thread_ = new std::thread(boost::bind(&SQLiteHistoryStorage::run, this));

    sqlite3_open(pathToString(file).c_str(), &db_);
    if (!db_) {
        std::cerr << "Error opening database " << pathToString(file) << std::endl;
    }

    char* errorMessage;
    int result = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS messages('message' STRING, 'fromBare' INTEGER, 'fromResource' STRING, 'toBare' INTEGER, 'toResource' STRING, 'type' INTEGER, 'time' INTEGER, 'offset' INTEGER)", nullptr, nullptr, &errorMessage);
    if (result != SQLITE_OK) {
        std::cerr << "SQL Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }

    result = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS jids('id' INTEGER PRIMARY KEY ASC AUTOINCREMENT, 'jid' STRING UNIQUE NOT NULL)", nullptr, nullptr, &errorMessage);
    if (result != SQLITE_OK) {
        std::cerr << "SQL Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
}

SQLiteHistoryStorage::~SQLiteHistoryStorage() {
    sqlite3_close(db_);
    delete thread_;
}

void SQLiteHistoryStorage::addMessage(const HistoryMessage& message) {
    int secondsSinceEpoch = (message.getTime() - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_seconds();

    std::string statement = std::string("INSERT INTO messages('message', 'fromBare', 'fromResource', 'toBare', 'toResource', 'type', 'time', 'offset') VALUES(") +
                    "'" + getEscapedString(message.getMessage()) + "', " +
                    boost::lexical_cast<std::string>(getIDForJID(message.getFromJID().toBare())) + ", '" +
                    getEscapedString(message.getFromJID().getResource()) + "', " +
                    boost::lexical_cast<std::string>(getIDForJID(message.getToJID().toBare())) + ", '" +
                    getEscapedString(message.getToJID().getResource()) + "', " +
                    boost::lexical_cast<std::string>(message.getType()) + ", " +
                    boost::lexical_cast<std::string>(secondsSinceEpoch) + ", " +
                    boost::lexical_cast<std::string>(message.getOffset()) + ")";
    char* errorMessage;
    int result = sqlite3_exec(db_, statement.c_str(), nullptr, nullptr, &errorMessage);
    if (result != SQLITE_OK) {
        std::cerr << "SQL Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
}

std::vector<HistoryMessage> SQLiteHistoryStorage::getMessagesFromDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    sqlite3_stmt* selectStatement;

    boost::optional<long long> selfID = getIDFromJID(selfJID.toBare());
    boost::optional<long long> contactID = getIDFromJID(contactJID.toBare());

    if (!selfID || !contactID) {
        // JIDs missing from the database
        return std::vector<HistoryMessage>();
    }

    std::string selectQuery = "SELECT * FROM messages WHERE (type=" + boost::lexical_cast<std::string>(type);
    if (contactJID.isBare()) {
        // match only bare jid
        selectQuery += " AND ((fromBare=" + boost::lexical_cast<std::string>(*selfID) + " AND toBare=" +
                boost::lexical_cast<std::string>(*contactID) + ") OR (fromBare=" +
                boost::lexical_cast<std::string>(*contactID) + " AND toBare=" + boost::lexical_cast<std::string>(*selfID) + ")))";
    }
    else {
        // match resource too
        selectQuery += " AND ((fromBare=" + boost::lexical_cast<std::string>(*selfID) + " AND (toBare=" +
                boost::lexical_cast<std::string>(*contactID) +" AND toResource='" +
                getEscapedString(contactJID.getResource()) + "')) OR ((fromBare=" +
                boost::lexical_cast<std::string>(*contactID) + " AND fromResource='" +
                getEscapedString(contactJID.getResource()) + "') AND toBare=" +
                boost::lexical_cast<std::string>(*selfID) + ")))";
    }

    if (!date.is_not_a_date()) {
        int lowerBound = (boost::posix_time::ptime(date) - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_seconds();
        int upperBound = lowerBound + 86400;

        selectQuery += " AND (time>=" + boost::lexical_cast<std::string>(lowerBound) +
                " AND time<" + boost::lexical_cast<std::string>(upperBound) + ")";
    }

    int r = sqlite3_prepare(db_, selectQuery.c_str(), boost::numeric_cast<int>(selectQuery.size()), &selectStatement, nullptr);
    if (r != SQLITE_OK) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }
    r = sqlite3_step(selectStatement);

    // Retrieve result
    std::vector<HistoryMessage> result;
    while (r == SQLITE_ROW) {
        std::string message(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 0)));

        // fromJID
        boost::optional<JID> fromJID(getJIDFromID(sqlite3_column_int(selectStatement, 1)));
        std::string fromResource(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 2)));
        if (fromJID) {
            fromJID = boost::optional<JID>(JID(fromJID->getNode(), fromJID->getDomain(), fromResource));
        }

        // toJID
        boost::optional<JID> toJID(getJIDFromID(sqlite3_column_int(selectStatement, 3)));
        std::string toResource(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 4)));
        if (toJID) {
            toJID = boost::optional<JID>(JID(toJID->getNode(), toJID->getDomain(), toResource));
        }

        // message type
        HistoryMessage::Type type = static_cast<HistoryMessage::Type>(sqlite3_column_int(selectStatement, 5));

        // timestamp
        int secondsSinceEpoch(sqlite3_column_int(selectStatement, 6));
        boost::posix_time::ptime time(boost::gregorian::date(1970, 1, 1), boost::posix_time::seconds(secondsSinceEpoch));

        // offset from utc
        int offset = sqlite3_column_int(selectStatement, 7);

        result.push_back(HistoryMessage(message, (fromJID ? *fromJID : JID()), (toJID ? *toJID : JID()), type, time, offset));
        r = sqlite3_step(selectStatement);
    }
    if (r != SQLITE_DONE) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }
    sqlite3_finalize(selectStatement);

    return result;
}

long long SQLiteHistoryStorage::getIDForJID(const JID& jid) {
    boost::optional<long long> id = getIDFromJID(jid);
    if (id) {
        return *id;
    }
    else {
        return addJID(jid);
    }
}

long long SQLiteHistoryStorage::addJID(const JID& jid) {
    std::string statement = std::string("INSERT INTO jids('jid') VALUES('") + getEscapedString(jid.toString()) + "')";
    char* errorMessage;
    int result = sqlite3_exec(db_, statement.c_str(), nullptr, nullptr, &errorMessage);
    if (result != SQLITE_OK) {
        std::cerr << "SQL Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
    return sqlite3_last_insert_rowid(db_);
}

boost::optional<JID> SQLiteHistoryStorage::getJIDFromID(long long id) const {
    boost::optional<JID> result;
    sqlite3_stmt* selectStatement;
    std::string selectQuery("SELECT jid FROM jids WHERE id=" + boost::lexical_cast<std::string>(id));
    int r = sqlite3_prepare(db_, selectQuery.c_str(), boost::numeric_cast<int>(selectQuery.size()), &selectStatement, nullptr);
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

boost::optional<long long> SQLiteHistoryStorage::getIDFromJID(const JID& jid) const {
    boost::optional<long long> result;
    sqlite3_stmt* selectStatement;
    std::string selectQuery("SELECT id FROM jids WHERE jid='" + jid.toString() + "'");
    long long r = sqlite3_prepare(db_, selectQuery.c_str(), boost::numeric_cast<int>(selectQuery.size()), &selectStatement, nullptr);
    if (r != SQLITE_OK) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }
    r = sqlite3_step(selectStatement);
    if (r == SQLITE_ROW) {
        result = boost::optional<long long>(sqlite3_column_int(selectStatement, 0));
    }
    sqlite3_finalize(selectStatement);
    return result;
}

ContactsMap SQLiteHistoryStorage::getContacts(const JID& selfJID, HistoryMessage::Type type, const std::string& keyword) const {
    ContactsMap result;
    sqlite3_stmt* selectStatement;

    // get id
    boost::optional<long long> id = getIDFromJID(selfJID);
    if (!id) {
        return result;
    }

    // get contacts
    std::string query = "SELECT DISTINCT messages.'fromBare', messages.'fromResource', messages.'toBare', messages.'toResource', messages.'time' "
        "FROM messages WHERE (type="
        + boost::lexical_cast<std::string>(type) + " AND (toBare="
        + boost::lexical_cast<std::string>(*id) + " OR fromBare=" + boost::lexical_cast<std::string>(*id) + "))";

    // match keyword
    if (getEscapedString(keyword).length()) {
        query += " AND message LIKE '%" + getEscapedString(keyword) + "%'";
    }

    int r = sqlite3_prepare(db_, query.c_str(), boost::numeric_cast<int>(query.size()), &selectStatement, nullptr);
    if (r != SQLITE_OK) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }

    r = sqlite3_step(selectStatement);
    while (r == SQLITE_ROW) {
        int fromBareID = sqlite3_column_int(selectStatement, 0);
        std::string fromResource(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 1)));
        int toBareID = sqlite3_column_int(selectStatement, 2);
        std::string toResource(reinterpret_cast<const char*>(sqlite3_column_text(selectStatement, 3)));
        std::string resource;

        int secondsSinceEpoch(sqlite3_column_int(selectStatement, 4));
        boost::posix_time::ptime time(boost::gregorian::date(1970, 1, 1), boost::posix_time::seconds(secondsSinceEpoch));

        boost::optional<JID> contactJID;

        if (fromBareID == *id) {
            contactJID = getJIDFromID(toBareID);
            resource = toResource;
        }
        else {
            contactJID = getJIDFromID(fromBareID);
            resource = fromResource;
        }

        // check if it is a MUC contact (from a private conversation)
        if (type == HistoryMessage::PrivateMessage) {
            contactJID = boost::optional<JID>(JID(contactJID->getNode(), contactJID->getDomain(), resource));
        }

        if (contactJID) {
            result[*contactJID].insert(time.date());
        }

        r = sqlite3_step(selectStatement);
    }

    if (r != SQLITE_DONE) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }
    sqlite3_finalize(selectStatement);

    return result;
}

boost::gregorian::date SQLiteHistoryStorage::getNextDateWithLogs(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date, bool reverseOrder) const {
    sqlite3_stmt* selectStatement;
    boost::optional<long long> selfID = getIDFromJID(selfJID.toBare());
    boost::optional<long long> contactID = getIDFromJID(contactJID.toBare());

    if (!selfID || !contactID) {
        // JIDs missing from the database
        return boost::gregorian::date(boost::gregorian::not_a_date_time);
    }

    std::string selectQuery = "SELECT time FROM messages WHERE (type=" + boost::lexical_cast<std::string>(type);
    if (contactJID.isBare()) {
        // match only bare jid
        selectQuery += " AND ((fromBare=" + boost::lexical_cast<std::string>(*selfID) + " AND toBare=" +
                boost::lexical_cast<std::string>(*contactID) + ") OR (fromBare=" +
                boost::lexical_cast<std::string>(*contactID) + " AND toBare=" + boost::lexical_cast<std::string>(*selfID) + ")))";
    }
    else {
        // match resource too
        selectQuery += " AND ((fromBare=" + boost::lexical_cast<std::string>(*selfID) + " AND (toBare=" +
                boost::lexical_cast<std::string>(*contactID) +" AND toResource='" +
                getEscapedString(contactJID.getResource()) + "')) OR ((fromBare=" +
                boost::lexical_cast<std::string>(*contactID) + " AND fromResource='" +
                getEscapedString(contactJID.getResource()) + "') AND toBare=" +
                boost::lexical_cast<std::string>(*selfID) + ")))";
    }

    int timeStamp = (boost::posix_time::ptime(date) - boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))).total_seconds() + (reverseOrder ? 0 : 86400);

    selectQuery += " AND time" + (reverseOrder ? std::string("<") : std::string(">")) + boost::lexical_cast<std::string>(timeStamp);
    selectQuery += " ORDER BY time " + (reverseOrder ? std::string("DESC") : std::string("ASC")) + " LIMIT 1";

    int r = sqlite3_prepare(db_, selectQuery.c_str(), boost::numeric_cast<int>(selectQuery.size()), &selectStatement, nullptr);
    if (r != SQLITE_OK) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }

    r = sqlite3_step(selectStatement);
    if (r == SQLITE_ROW) {
        int secondsSinceEpoch(sqlite3_column_int(selectStatement, 0));
        boost::posix_time::ptime time(boost::gregorian::date(1970, 1, 1), boost::posix_time::seconds(secondsSinceEpoch));
        std::cout << "next day is: " << time.date() << "\n";
        return time.date();
    }

    return boost::gregorian::date(boost::gregorian::not_a_date_time);
}

std::vector<HistoryMessage> SQLiteHistoryStorage::getMessagesFromNextDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    boost::gregorian::date nextDate = getNextDateWithLogs(selfJID, contactJID, type, date, false);

    if (nextDate.is_not_a_date()) {
        return std::vector<HistoryMessage>();
    }

    return getMessagesFromDate(selfJID, contactJID, type, nextDate);
}

std::vector<HistoryMessage> SQLiteHistoryStorage::getMessagesFromPreviousDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    boost::gregorian::date previousDate = getNextDateWithLogs(selfJID, contactJID, type, date, true);

    if (previousDate.is_not_a_date()) {
        return std::vector<HistoryMessage>();
    }

    return getMessagesFromDate(selfJID, contactJID, type, previousDate);
}

boost::posix_time::ptime SQLiteHistoryStorage::getLastTimeStampFromMUC(const JID& selfJID, const JID& mucJID) const {
    boost::optional<long long> selfID = getIDFromJID(selfJID.toBare());
    boost::optional<long long> mucID = getIDFromJID(mucJID.toBare());

    if (!selfID || !mucID) {
        // JIDs missing from the database
        return boost::posix_time::ptime(boost::posix_time::not_a_date_time);
    }


    sqlite3_stmt* selectStatement;
    std::string selectQuery = "SELECT messages.'time', messages.'offset' from messages WHERE type=1 AND (toBare=" +
                boost::lexical_cast<std::string>(*selfID) + " AND fromBare=" +
                boost::lexical_cast<std::string>(*mucID) + ") ORDER BY time DESC LIMIT 1";

    int r = sqlite3_prepare(db_, selectQuery.c_str(), boost::numeric_cast<int>(selectQuery.size()), &selectStatement, nullptr);
    if (r != SQLITE_OK) {
        std::cout << "Error: " << sqlite3_errmsg(db_) << std::endl;
    }

    r = sqlite3_step(selectStatement);
    if (r == SQLITE_ROW) {
        int secondsSinceEpoch(sqlite3_column_int(selectStatement, 0));
        boost::posix_time::ptime time(boost::gregorian::date(1970, 1, 1), boost::posix_time::seconds(secondsSinceEpoch));
        int offset = sqlite3_column_int(selectStatement, 1);

        return time - boost::posix_time::hours(offset);
    }

    return boost::posix_time::ptime(boost::posix_time::not_a_date_time);
}

void SQLiteHistoryStorage::run() {
}

}
