#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/ELements/ChatState.h"

namespace Swift {
	class ChatStateTracker {
		public:
			ChatStateTracker();
			void handleMessageReceived(boost::shared_ptr<Message> message);
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> oldPresence);
			boost::signal<void (ChatState::ChatStateType)> onChatStateChange;
		private:
			void changeState(ChatState::ChatStateType state);
			ChatState::ChatStateType currentState_;
	};
}
