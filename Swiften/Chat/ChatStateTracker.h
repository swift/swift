/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/ChatState.h>

namespace Swift {
	class ChatStateTracker {
		public:
			ChatStateTracker();
			void handleMessageReceived(boost::shared_ptr<Message> message);
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
			boost::signal<void (ChatState::ChatStateType)> onChatStateChange;
		private:
			void changeState(ChatState::ChatStateType state);
			ChatState::ChatStateType currentState_;
	};
}
