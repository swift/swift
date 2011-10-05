/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {
	class JoinMUCUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<JoinMUCUIEvent> ref;
			JoinMUCUIEvent(const JID& jid, const boost::optional<std::string>& password = boost::optional<std::string>(), const boost::optional<std::string>& nick = boost::optional<std::string>(), bool joinAutomaticallyInFuture = false, bool createAsReservedRoomIfNew = false) : jid_(jid), nick_(nick), joinAutomatically_(joinAutomaticallyInFuture), createAsReservedRoomIfNew_(createAsReservedRoomIfNew), password_(password) {};
			const boost::optional<std::string>& getNick() const {return nick_;}
			const JID& getJID() const {return jid_;}
			bool getShouldJoinAutomatically() const {return joinAutomatically_;}
			bool getCreateAsReservedRoomIfNew() const {return createAsReservedRoomIfNew_;}
			const boost::optional<std::string>& getPassword() const {return password_;}
		private:
			JID jid_;
			boost::optional<std::string> nick_;
			bool joinAutomatically_;
			bool createAsReservedRoomIfNew_;
			boost::optional<std::string> password_;
	};
}
