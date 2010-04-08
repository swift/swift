/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

//Not used yet.

#pragma once

#include "Swiften/Base/String.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class JoinMUCUIEvent : public UIEvent {
		public:
			JoinMUCUIEvent(const JID& jid, const String& contact) : jid_(jid), contact_(contact) {};
			String getContact() {return contact_;};
			JID getJID() {return jid_;};
		private:
			String contact_;
			JID jid_;
	};
}
