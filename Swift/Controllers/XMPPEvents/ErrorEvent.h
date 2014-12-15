/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>

#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/XMPPEvents/StanzaEvent.h"
#include <string>
#include "Swiften/JID/JID.h"

namespace Swift {
	class ErrorEvent : public StanzaEvent {
		public:
			ErrorEvent(const JID& jid, const std::string& text) : jid_(jid), text_(text){}
			virtual ~ErrorEvent(){}
			const JID& getJID() const {return jid_;}
			const std::string& getText() const {return text_;}

		private:
			JID jid_;
			std::string text_;
	};
}

