#pragma once

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class ChatStateNotifier {
		public:
			enum ChatState {Active, Composing, Paused, Inactive, Gone};
			ChatStateNotifier();
			void setContactHas85Caps(bool hasCaps);
			void setUserIsTyping();
			void userSentMessage();
			void userCancelledNewMessage();
			void receivedMessageFromContact(bool hasActiveElement);
			bool contactShouldReceiveStates();

			boost::signal<void (ChatState)> onChatStateChanged;
		private:
			bool contactHas85Caps_;
			bool isInConversation_;
			bool contactHasSentActive_;
			bool userIsTyping_;
	};
}
