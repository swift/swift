/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>
#include <map>
#include <vector>
#include <Swiften/JID/JID.h>
#include <Swiften/History/HistoryMessage.h>
#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace Swift {
	typedef std::map<JID, std::set<boost::gregorian::date> > ContactsMap;

	class HistoryStorage {
		/**
		 * Messages are stored using localtime timestamps.
		 */
		public:
			virtual ~HistoryStorage() {}

			virtual void addMessage(const HistoryMessage& message) = 0;
			virtual std::vector<HistoryMessage> getMessagesFromDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const = 0;
			virtual std::vector<HistoryMessage> getMessagesFromNextDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const = 0;
			virtual std::vector<HistoryMessage> getMessagesFromPreviousDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const = 0;
			virtual ContactsMap getContacts(const JID& selfJID, HistoryMessage::Type type, const std::string& keyword) const = 0;
			virtual boost::posix_time::ptime getLastTimeStampFromMUC(const JID& selfJID, const JID& mucJID) const = 0;
	};
}
