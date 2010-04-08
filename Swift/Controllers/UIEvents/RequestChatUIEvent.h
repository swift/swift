/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"

#include "Swift/Controllers/UIEvents/UIEvent.h"

namespace Swift {
	class RequestChatUIEvent : public UIEvent {
		public:
			RequestChatUIEvent(const JID& contact) : contact_(contact) {};
			JID getContact() {return contact_;}
		private:
			JID contact_;
	};
}
