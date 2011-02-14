/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class JoinMUCUIEvent : public UIEvent {
		public:
			typedef boost::shared_ptr<JoinMUCUIEvent> ref;
			JoinMUCUIEvent(const JID& jid, const boost::optional<std::string>& nick = boost::optional<std::string>()) : jid_(jid), nick_(nick) {};
			boost::optional<std::string> getNick() {return nick_;};
			JID getJID() {return jid_;};
		private:
			JID jid_;
			boost::optional<std::string> nick_;
	};
}
