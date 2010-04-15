/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include "Swiften/Base/String.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class JoinMUCUIEvent : public UIEvent {
		public:
			JoinMUCUIEvent(const JID& jid, const boost::optional<String>& nick) : jid_(jid), nick_(nick) {};
			boost::optional<String> getNick() {return nick_;};
			JID getJID() {return jid_;};
		private:
			JID jid_;
			boost::optional<String> nick_;
	};
}
