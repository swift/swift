/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatController.h"

#include <boost/bind.hpp>
#include <stdio.h>

#include "Swiften/Avatars/AvatarManager.h"
#include "Swiften/Chat/ChatStateNotifier.h"
#include "Swiften/Chat/ChatStateMessageSender.h"
#include "Swiften/Chat/ChatStateTracker.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Disco/EntityCapsManager.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsManager* entityCapsManager)
	: ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager, useDelayForLatency, eventStream, eventController, timerFactory) {
	isInMUC_ = isInMUC;
	lastWasPresence_ = false;
	entityCapsManager_ = entityCapsManager;
	chatStateNotifier_ = new ChatStateNotifier();
	entityCapsManager_->onCapsChanged.connect(boost::bind(&ChatController::handleCapsChanged, this, _1));
	handleCapsChanged(toJID_);
	chatStateMessageSender_ = new ChatStateMessageSender(chatStateNotifier_, stanzaChannel, contact);
	chatStateTracker_ = new ChatStateTracker();
	nickResolver_ = nickResolver;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatController::handlePresenceChange, this, _1));
	chatStateTracker_->onChatStateChange.connect(boost::bind(&ChatWindow::setContactChatState, chatWindow_, _1));
	stanzaChannel_->onStanzaAcked.connect(boost::bind(&ChatController::handleStanzaAcked, this, _1));
	String nick = nickResolver_->jidToNick(toJID_);
	chatWindow_->setName(nick);
	String startMessage("Starting chat with " + nick);
	Presence::ref theirPresence;
	if (isInMUC) {
		startMessage += " in chatroom " + contact.toBare().toString();
		theirPresence = presenceOracle->getLastPresence(contact);
	} else {
		startMessage += " - " + contact.toBare().toString();
		theirPresence = contact.isBare() ? presenceOracle->getHighestPriorityPresence(contact.toBare()) : presenceOracle->getLastPresence(contact);
	}
	startMessage += ": " + StatusShow::typeToFriendlyName(theirPresence ? theirPresence->getShow() : StatusShow::None);
	if (theirPresence && !theirPresence->getStatus().isEmpty()) {
		startMessage += " (" + theirPresence->getStatus() + ")";
	}
	chatStateNotifier_->setContactIsOnline(theirPresence && theirPresence->getType() == Presence::Available);
	startMessage += ".";
	chatWindow_->addSystemMessage(startMessage);
	chatWindow_->onUserTyping.connect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
	chatWindow_->onUserCancelsTyping.connect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));

}

ChatController::~ChatController() {
	delete chatStateNotifier_;
	delete chatStateMessageSender_;
	delete chatStateTracker_;
}

void ChatController::handleCapsChanged(const JID& jid) {
	if (jid == toJID_) {
		DiscoInfo::ref caps = entityCapsManager_->getCaps(toJID_);
		bool hasCSN = caps && caps->hasFeature(ChatState::getFeatureNamespace());
		chatStateNotifier_->setContactHas85Caps(hasCSN);
	}
}

void ChatController::setToJID(const JID& jid) {
	chatStateNotifier_->contactJIDHasChanged();
	chatStateMessageSender_->setContact(jid);
	ChatControllerBase::setToJID(jid);
	handleCapsChanged(jid);
	Presence::ref presence;
	if (isInMUC_) {
		presence = presenceOracle_->getLastPresence(jid);
	} else {
		presence = jid.isBare() ? presenceOracle_->getHighestPriorityPresence(jid.toBare()) : presenceOracle_->getLastPresence(jid);
	}
	chatStateNotifier_->setContactIsOnline(presence && presence->getType() == Presence::Available);
}

bool ChatController::isIncomingMessageFromMe(boost::shared_ptr<Message>) {
	return false;
}

void ChatController::preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	eventController_->handleIncomingEvent(messageEvent);
	if (messageEvent->isReadable()) {
		chatWindow_->flash();
	}
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	JID from = message->getFrom();
	if (!from.equals(toJID_, JID::WithResource)) {
		if (toJID_.equals(from, JID::WithoutResource)  && toJID_.isBare()){
			setToJID(from);
		}
	}
	chatStateNotifier_->receivedMessageFromContact(message->getPayload<ChatState>());
	chatStateTracker_->handleMessageReceived(message);
	lastWasPresence_ = false;
}

void ChatController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	if (chatStateNotifier_->contactShouldReceiveStates()) {
		message->addPayload(boost::shared_ptr<Payload>(new ChatState(ChatState::Active)));
	}
}

void ChatController::postSendMessage(const String& body, boost::shared_ptr<Stanza> sentStanza) {
	String id = addMessage(body, "me", true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel() : boost::optional<SecurityLabel>(), String(avatarManager_->getAvatarPath(selfJID_).string()), boost::posix_time::microsec_clock::universal_time());
	if (stanzaChannel_->getStreamManagementEnabled()) {
		chatWindow_->setAckState(id, ChatWindow::Pending);
		unackedStanzas_[sentStanza] = id;
	}
	lastWasPresence_ = false;
	chatStateNotifier_->userSentMessage();
}

void ChatController::handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
	String id = unackedStanzas_[stanza];
	if (id != "") {
		chatWindow_->setAckState(id, ChatWindow::Received);
	}
	unackedStanzas_.erase(unackedStanzas_.find(stanza));
}

void ChatController::setOnline(bool online) {
	if (!online) {
		std::map<boost::shared_ptr<Stanza>, String>::iterator it = unackedStanzas_.begin();
		for ( ; it != unackedStanzas_.end(); it++) {
			chatWindow_->setAckState(it->second, ChatWindow::Failed);
		}
		unackedStanzas_.clear();

		Presence::ref fakeOffline(new Presence());
		fakeOffline->setFrom(toJID_);
		fakeOffline->setType(Presence::Unavailable);
		chatStateTracker_->handlePresenceChange(fakeOffline);
	}
	ChatControllerBase::setOnline(online);
}

String ChatController::senderDisplayNameFromMessage(const JID& from) {
	return nickResolver_->jidToNick(from);
}

String ChatController::getStatusChangeString(boost::shared_ptr<Presence> presence) {
	String nick = senderDisplayNameFromMessage(presence->getFrom());
	String response = nick;
	if (presence->getType() == Presence::Unavailable) {
		response += " has gone offline";
	} else if (presence->getType() == Presence::Available) {
		StatusShow::Type show = presence->getShow();
		if (show == StatusShow::Online || show == StatusShow::FFC) {
			response += " has become available";
		} else if (show == StatusShow::Away || show == StatusShow::XA) {
			response += " has gone away";
		} else if (show == StatusShow::DND) {
			response += " is now busy";
		} 
	}
	if (!presence->getStatus().isEmpty()) {
		response += " (" + presence->getStatus() + ")";
	}
	return response + ".";
}

void ChatController::handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
	if (!toJID_.equals(newPresence->getFrom(), toJID_.isBare() ? JID::WithoutResource : JID::WithResource)) {
		return;
	}

	chatStateTracker_->handlePresenceChange(newPresence);
	chatStateNotifier_->setContactIsOnline(newPresence->getType() == Presence::Available);
	String newStatusChangeString = getStatusChangeString(newPresence);
	if (newStatusChangeString != lastStatusChangeString_) {
		if (lastWasPresence_) {
			chatWindow_->replaceLastMessage(newStatusChangeString);
		} else {
			chatWindow_->addPresenceMessage(newStatusChangeString);
		}
		lastStatusChangeString_ = newStatusChangeString;
		lastWasPresence_ = true;
	}
}

boost::optional<boost::posix_time::ptime> ChatController::getMessageTimestamp(boost::shared_ptr<Message> message) const {
	return message->getTimestamp();
}

}
