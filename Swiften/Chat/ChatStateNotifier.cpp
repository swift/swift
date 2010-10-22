/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Chat/ChatStateNotifier.h"

namespace Swift {

ChatStateNotifier::ChatStateNotifier() {
	contactJIDHasChanged();
}

void ChatStateNotifier::setContactHas85Caps(bool hasCaps) {
	contactHas85Caps_ = hasCaps;
}

void ChatStateNotifier::setContactIsOnline(bool online) {
	contactIsOnline_ = online;
}

void ChatStateNotifier::contactJIDHasChanged() {
	contactHasSentActive_ = false;
	contactHas85Caps_ = false;
	userIsTyping_ = false;
	contactIsOnline_ = false;
}

void ChatStateNotifier::setUserIsTyping() {
	bool should = contactShouldReceiveStates();
	if (should && !userIsTyping_) {
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
	contactHasSentActive_ = hasActiveElement;
}

bool ChatStateNotifier::contactShouldReceiveStates() {
	/* So, yes, the XEP says to look at caps, but it also says that once you've
	   heard from the contact, the active state overrides this.
	   *HOWEVER* it says that the MUST NOT send csn if you haven't received
	   active is OPTIONAL behaviour for if you haven't got caps.*/
	return contactIsOnline_ && (contactHasSentActive_ || contactHas85Caps_);
}

}
