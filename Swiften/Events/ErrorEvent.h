/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <cassert>

#include "Swiften/Base/boost_signalslib.h"
#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class ErrorEvent : public StanzaEvent {
		public:
			ErrorEvent(const JID& jid, const String& text) : jid_(jid), text_(text){};
			virtual ~ErrorEvent(){};
			const JID& getJID() const {return jid_;};
			const String& getText() const {return text_;};

		private:
			JID jid_;
			String text_;
	};
}

