#include "Swiften/Chat/ChatStateNotifier.h"

namespace Swift {

ChatStateNotifier::ChatStateNotifier() {
	contactHas85Caps_ = false;
	isInConversation_ = false;
	contactHasSentActive_ = false;
	userIsTyping_ = false;
}

void ChatStateNotifier::setContactHas85Caps(bool hasCaps) {
	contactHas85Caps_ = hasCaps;
}

void ChatStateNotifier::setUserIsTyping() {
	if (contactShouldReceiveStates() && !userIsTyping_) {
		userIsTyping_ = true;
		onChatStateChanged(ChatState::Composing);
	}
}

void ChatStateNotifier::userSentMessage() {
	userIsTyping_ = false;
}

void ChatStateNotifier::userCancelledNewMessage() {
	if (userIsTyping_) {
		userIsTyping_ = false;
		onChatStateChanged(ChatState::Active);
	}
}

void ChatStateNotifier::receivedMessageFromContact(bool hasActiveElement) {
	isInConversation_ = true;
	contactHasSentActive_ = hasActiveElement;
}

bool ChatStateNotifier::contactShouldReceiveStates() {
	/* So, yes, the XEP says to look at caps, but it also says that once you've
	   heard from the contact, the active state overrides this.*/
	return contactHasSentActive_ || (contactHas85Caps_ && !isInConversation_);;
}

}
