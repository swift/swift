#include "Swiften/Chat/ChatStateTracker.h"

namespace Swift {
ChatStateTracker::ChatStateTracker() {
	currentState_ = ChatState::Gone;
}

void ChatStateTracker::handleMessageReceived(boost::shared_ptr<Message> message) {
	boost::shared_ptr<ChatState> statePayload = message->getPayload<ChatState>();
	if (statePayload) {
		changeState(statePayload->getChatState());;
	}
}

void ChatStateTracker::handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence>) {
	if (newPresence->getType() == Presence::Unavailable) {
		onChatStateChange(ChatState::Gone);
	}
}

void ChatStateTracker::changeState(ChatState::ChatStateType state) {
	printf("Comparing state %d to old state %d\n", state, currentState_);
	if (state != currentState_) {
		currentState_ = state;
		onChatStateChange(state);
	}
}

}
