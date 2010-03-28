#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/ChatState.h"

namespace Swift {
	class ChatStateNotifier {
		public:
			ChatStateNotifier();
			void setContactHas85Caps(bool hasCaps);
			void setUserIsTyping();
			void userSentMessage();
			void userCancelledNewMessage();
			void receivedMessageFromContact(bool hasActiveElement);
			bool contactShouldReceiveStates();

			boost::signal<void (ChatState::ChatStateType)> onChatStateChanged;
		private:
			bool contactHas85Caps_;
			bool isInConversation_;
			bool contactHasSentActive_;
			bool userIsTyping_;
	};
}
