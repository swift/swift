/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Chat/ChatStateNotifier.h"
#include "Swiften/Elements/ChatState.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class StanzaChannel;
	class ChatStateMessageSender {
		public:
			ChatStateMessageSender(ChatStateNotifier* notifier, StanzaChannel* stanzaChannel, const JID& contact);
			void setContact(const JID& contact) {contact_ = contact;};

		private:
			void handleChatStateChanged(ChatState::ChatStateType state);
			ChatStateNotifier* notifier_;
			StanzaChannel* stanzaChannel_;
			JID contact_;
	};
}
