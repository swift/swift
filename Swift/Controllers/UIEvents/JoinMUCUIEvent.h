/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
    class JoinMUCUIEvent : public UIEvent {
        public:
            typedef std::shared_ptr<JoinMUCUIEvent> ref;
            JoinMUCUIEvent(const JID& jid, const boost::optional<std::string>& password = boost::optional<std::string>(), const boost::optional<std::string>& nick = boost::optional<std::string>(), bool createAsReservedRoomIfNew = false, bool isImpromptu = false, bool isContinuation = false) : jid_(jid), nick_(nick), createAsReservedRoomIfNew_(createAsReservedRoomIfNew), password_(password), isImpromptuMUC_(isImpromptu), isContinuation_(isContinuation) {}
            const boost::optional<std::string>& getNick() const {return nick_;}
            const JID& getJID() const {return jid_;}
            bool getShouldJoinAutomatically() const {return joinAutomatically_;}
            bool getCreateAsReservedRoomIfNew() const {return createAsReservedRoomIfNew_;}
            const boost::optional<std::string>& getPassword() const {return password_;}
            bool isImpromptu() const {return isImpromptuMUC_;}
            bool isContinuation() const {return isContinuation_;}

        private:
            JID jid_;
            boost::optional<std::string> nick_;
            bool joinAutomatically_;
            bool createAsReservedRoomIfNew_;
            boost::optional<std::string> password_;
            bool isImpromptuMUC_;
            bool isContinuation_;
    };
}
