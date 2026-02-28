/*
 * Copyright (c) 2012 Catalin Badea
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swift/Controllers/HistoryController.h>

#include <boost/date_time/c_local_time_adjustor.hpp>

#include <Swiften/History/HistoryMessage.h>
#include <Swiften/History/HistoryStorage.h>
#include <Swiften/JID/JID.h>

namespace Swift {

HistoryController::HistoryController(HistoryStorage* localHistoryStorage) : localHistory_(localHistoryStorage) {
}

HistoryController::~HistoryController() {
}

void HistoryController::addMessage(const std::string& message, const JID& fromJID, const JID& toJID, HistoryMessage::Type type, const boost::posix_time::ptime& timeStamp) {
    // note: using localtime timestamps
    boost::posix_time::ptime localTime = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(timeStamp);
    int offset = (localTime - timeStamp).hours();

    HistoryMessage historyMessage(message, fromJID, toJID, type, localTime, offset);

    localHistory_->addMessage(historyMessage);
    onNewMessage(historyMessage);
}

std::vector<HistoryMessage> HistoryController::getMessagesFromDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    return localHistory_->getMessagesFromDate(selfJID, contactJID, type, date);
}

std::vector<HistoryMessage> HistoryController::getMUCContext(const JID& selfJID, const JID& mucJID, const boost::posix_time::ptime& timeStamp) const {
    boost::posix_time::ptime localTime = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(timeStamp);
    return getMessagesFromDate(selfJID, mucJID, HistoryMessage::Groupchat, localTime.date());
}

std::vector<HistoryMessage> HistoryController::getMessagesFromPreviousDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    return localHistory_->getMessagesFromPreviousDate(selfJID, contactJID, type, date);
}

std::vector<HistoryMessage> HistoryController::getMessagesFromNextDate(const JID& selfJID, const JID& contactJID, HistoryMessage::Type type, const boost::gregorian::date& date) const {
    return localHistory_->getMessagesFromNextDate(selfJID, contactJID, type, date);
}

ContactsMap HistoryController::getContacts(const JID& selfJID, HistoryMessage::Type type, const std::string& keyword) const {
    return localHistory_->getContacts(selfJID, type, keyword);
}

boost::posix_time::ptime HistoryController::getLastTimeStampFromMUC(const JID& selfJID, const JID& mucJID) {
    return localHistory_->getLastTimeStampFromMUC(selfJID, mucJID);
}

}
