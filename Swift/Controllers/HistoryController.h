/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <set>
#include <Swiften/Base/boost_bsignals.h>
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

			boost::signal<void (const HistoryMessage&)> onNewMessage;

		private:
			HistoryStorage* localHistory_;
			bool remoteArchiveSupported_;
	};
}
