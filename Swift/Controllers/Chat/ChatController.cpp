/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatController.h"

#include <boost/bind.hpp>

#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Chat/ChatStateNotifier.h"
#include "Swiften/Chat/ChatStateMessageSender.h"
#include "Swiften/Chat/ChatStateTracker.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/EventController.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController)
	: ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager, useDelayForLatency, eventStream, eventController) {
	isInMUC_ = isInMUC;
	chatStateNotifier_ = new ChatStateNotifier();
	chatStateMessageSender_ = new ChatStateMessageSender(chatStateNotifier_, stanzaChannel, contact);
	chatStateTracker_ = new ChatStateTracker();
	nickResolver_ = nickResolver;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatController::handlePresenceChange, this, _1, _2));
	chatStateTracker_->onChatStateChange.connect(boost::bind(&ChatWindow::setContactChatState, chatWindow_, _1));
	String nick = nickResolver_->jidToNick(toJID_);
	chatWindow_->setName(nick);
	String startMessage("Starting chat with " + nick);
	if (isInMUC) {
		startMessage += " in chatroom " + contact.toBare().toString() + ".";
	} else {
		startMessage += " (" + contact.toBare().toString() + ").";
	}
	chatWindow_->addSystemMessage(startMessage);
	chatWindow_->onUserTyping.connect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
	chatWindow_->onUserCancelsTyping.connect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
}

ChatController::~ChatController() {
	delete chatStateNotifier_;
	delete chatStateMessageSender_;
	delete chatStateTracker_;
}

void ChatController::setToJID(const JID& jid) {
	chatStateMessageSender_->setContact(jid);
	ChatControllerBase::setToJID(jid);
}

bool ChatController::isIncomingMessageFromMe(boost::shared_ptr<Message>) {
	return false;
}

void ChatController::preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	eventController_->handleIncomingEvent(messageEvent);
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	JID from = message->getFrom();
	if (!from.equals(toJID_, JID::WithResource)) {
		if (toJID_.equals(from, JID::WithoutResource)  && toJID_.isBare()){
			toJID_ = from;
		}
	}
	chatStateNotifier_->receivedMessageFromContact(message->getPayload<ChatState>());
	chatStateTracker_->handleMessageReceived(message);
}

void ChatController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	if (chatStateNotifier_->contactShouldReceiveStates()) {
		message->addPayload(boost::shared_ptr<Payload>(new ChatState(ChatState::Active)));
	}
}

void ChatController::postSendMessage(const String& body) {
	addMessage(body, "me", true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel() : boost::optional<SecurityLabel>(), String(avatarManager_->getAvatarPath(selfJID_).string()), boost::posix_time::microsec_clock::universal_time());
	chatStateNotifier_->userSentMessage();
}

String ChatController::senderDisplayNameFromMessage(const JID& from) {
	return nickResolver_->jidToNick(from);
}

String ChatController::getStatusChangeString(boost::shared_ptr<Presence> presence) {
	String nick = senderDisplayNameFromMessage(presence->getFrom());
	if (presence->getType() == Presence::Unavailable) {
		return nick + " has gone offline.";
	} else if (presence->getType() == Presence::Available) {
		StatusShow::Type show = presence->getShow();
		if (show == StatusShow::Online || show == StatusShow::FFC) {
			return nick + " has become available.";
		} else if (show == StatusShow::Away || show == StatusShow::XA) {
			return nick + " has gone away.";
		} else if (show == StatusShow::DND) {
			return nick + " is now busy.";
		} 
	}

	return "";
}

void ChatController::handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> previousPresence) {
	if (!toJID_.equals(newPresence->getFrom(), toJID_.isBare() ? JID::WithoutResource : JID::WithResource)) {
		return;
	}

	chatStateTracker_->handlePresenceChange(newPresence, previousPresence);
	String newStatusChangeString = getStatusChangeString(newPresence);
	if (!previousPresence || newStatusChangeString != getStatusChangeString(previousPresence)) {
		chatWindow_->addSystemMessage(newStatusChangeString);
	}
}

boost::optional<boost::posix_time::ptime> ChatController::getMessageTimestamp(boost::shared_ptr<Message> message) const {
	return message->getTimestamp();
}

}
