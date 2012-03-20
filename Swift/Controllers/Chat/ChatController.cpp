/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatController.h"

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <stdio.h>

#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/format.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Chat/ChatStateNotifier.h>
#include <Swiften/Chat/ChatStateTracker.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swiften/Client/NickResolver.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/StatusUtil.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swift/Controllers/SettingConstants.h>

#include <Swiften/Base/Log.h>

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, SettingsProvider* settings)
	: ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager, useDelayForLatency, eventStream, eventController, timerFactory, entityCapsProvider), eventStream_(eventStream), userWantsReceipts_(userWantsReceipts), settings_(settings) {
	isInMUC_ = isInMUC;
	lastWasPresence_ = false;
	chatStateNotifier_ = new ChatStateNotifier(stanzaChannel, contact, entityCapsProvider);
	chatStateTracker_ = new ChatStateTracker();
	nickResolver_ = nickResolver;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatController::handlePresenceChange, this, _1));
	chatStateTracker_->onChatStateChange.connect(boost::bind(&ChatWindow::setContactChatState, chatWindow_, _1));
	stanzaChannel_->onStanzaAcked.connect(boost::bind(&ChatController::handleStanzaAcked, this, _1));
	nickResolver_->onNickChanged.connect(boost::bind(&ChatController::handleContactNickChanged, this, _1, _2));
	std::string nick = nickResolver_->jidToNick(toJID_);
	chatWindow_->setName(nick);
	std::string startMessage;
	Presence::ref theirPresence;
	if (isInMUC) {
		startMessage = str(format(QT_TRANSLATE_NOOP("", "Starting chat with %1% in chatroom %2%")) % nick % contact.toBare().toString());
		theirPresence = presenceOracle->getLastPresence(contact);
	} else {
		startMessage = str(format(QT_TRANSLATE_NOOP("", "Starting chat with %1% - %2%")) % nick % contact.toBare().toString());
		theirPresence = contact.isBare() ? presenceOracle->getHighestPriorityPresence(contact.toBare()) : presenceOracle->getLastPresence(contact);
	}
	startMessage += ": " + statusShowTypeToFriendlyName(theirPresence ? theirPresence->getShow() : StatusShow::None);
	if (theirPresence && !theirPresence->getStatus().empty()) {
		startMessage += " (" + theirPresence->getStatus() + ")";
	}
	lastShownStatus_ = theirPresence ? theirPresence->getShow() : StatusShow::None;
	chatStateNotifier_->setContactIsOnline(theirPresence && theirPresence->getType() == Presence::Available);
	startMessage += ".";
	chatWindow_->addSystemMessage(startMessage);
	chatWindow_->onUserTyping.connect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
	chatWindow_->onUserCancelsTyping.connect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
	chatWindow_->onFileTransferStart.connect(boost::bind(&ChatController::handleFileTransferStart, this, _1, _2));
	chatWindow_->onFileTransferAccept.connect(boost::bind(&ChatController::handleFileTransferAccept, this, _1, _2));
	chatWindow_->onFileTransferCancel.connect(boost::bind(&ChatController::handleFileTransferCancel, this, _1));
	chatWindow_->onSendFileRequest.connect(boost::bind(&ChatController::handleSendFileRequest, this, _1));
	handleBareJIDCapsChanged(toJID_);

	settings_->onSettingChanged.connect(boost::bind(&ChatController::handleSettingChanged, this, _1));
}

void ChatController::handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/) {
	if (jid.toBare() == toJID_.toBare()) {
		chatWindow_->setName(nickResolver_->jidToNick(jid));
	}
}

ChatController::~ChatController() {
	settings_->onSettingChanged.disconnect(boost::bind(&ChatController::handleSettingChanged, this, _1));
	nickResolver_->onNickChanged.disconnect(boost::bind(&ChatController::handleContactNickChanged, this, _1, _2));
	delete chatStateNotifier_;
	delete chatStateTracker_;
}

void ChatController::handleBareJIDCapsChanged(const JID& /*jid*/) {
	DiscoInfo::ref disco = entityCapsProvider_->getCaps(toJID_);
	if (disco) {
		if (disco->hasFeature(DiscoInfo::MessageCorrectionFeature)) {
			chatWindow_->setCorrectionEnabled(ChatWindow::Yes);
		} else {
			chatWindow_->setCorrectionEnabled(ChatWindow::No);
		}
		if (disco->hasFeature(DiscoInfo::MessageDeliveryReceiptsFeature)) {
			contactSupportsReceipts_ = ChatWindow::Yes;
		} else {
			contactSupportsReceipts_ = ChatWindow::No;
		}
	} else {
		SWIFT_LOG(debug) << "No disco info :(" << std::endl;
		chatWindow_->setCorrectionEnabled(ChatWindow::Maybe);
		contactSupportsReceipts_ = ChatWindow::Maybe;
	}
	checkForDisplayingDisplayReceiptsAlert();
}

void ChatController::setToJID(const JID& jid) {
	chatStateNotifier_->setContact(jid);
	ChatControllerBase::setToJID(jid);
	Presence::ref presence;
	if (isInMUC_) {
		presence = presenceOracle_->getLastPresence(jid);
	} else {
		presence = jid.isBare() ? presenceOracle_->getHighestPriorityPresence(jid.toBare()) : presenceOracle_->getLastPresence(jid);
	}
	chatStateNotifier_->setContactIsOnline(presence && presence->getType() == Presence::Available);
	handleBareJIDCapsChanged(toJID_);
}

bool ChatController::isIncomingMessageFromMe(boost::shared_ptr<Message>) {
	return false;
}

void ChatController::preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	if (messageEvent->isReadable()) {
		chatWindow_->flash();
		lastWasPresence_ = false;
	}
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	JID from = message->getFrom();
	if (!from.equals(toJID_, JID::WithResource)) {
		if (toJID_.equals(from, JID::WithoutResource)  && toJID_.isBare()){
			setToJID(from);
		}
	}
	chatStateTracker_->handleMessageReceived(message);
	chatStateNotifier_->receivedMessageFromContact(message->getPayload<ChatState>());

	if (boost::shared_ptr<DeliveryReceipt> receipt = message->getPayload<DeliveryReceipt>()) {
		SWIFT_LOG(debug) << "received receipt for id: " << receipt->getReceivedID() << std::endl;
		if (requestedReceipts_.find(receipt->getReceivedID()) != requestedReceipts_.end()) {
			chatWindow_->setMessageReceiptState(requestedReceipts_[receipt->getReceivedID()], ChatWindow::ReceiptReceived);
			requestedReceipts_.erase(receipt->getReceivedID());
		}
	} else if (message->getPayload<DeliveryReceiptRequest>()) {
		if (receivingPresenceFromUs_) {
			boost::shared_ptr<Message> receiptMessage = boost::make_shared<Message>();
			receiptMessage->setTo(toJID_);
			receiptMessage->addPayload(boost::make_shared<DeliveryReceipt>(message->getID()));
			stanzaChannel_->sendMessage(receiptMessage);
		}
	}
}

void ChatController::postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	eventController_->handleIncomingEvent(messageEvent);
}


void ChatController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	chatStateNotifier_->addChatStateRequest(message);
	if (userWantsReceipts_ && (contactSupportsReceipts_ != ChatWindow::No) && message) {
		message->addPayload(boost::make_shared<DeliveryReceiptRequest>());
	}
}

void ChatController::setContactIsReceivingPresence(bool isReceivingPresence) {
	receivingPresenceFromUs_ = isReceivingPresence;
}

void ChatController::handleSettingChanged(const std::string& settingPath) {
	if (settingPath == SettingConstants::REQUEST_DELIVERYRECEIPTS.getKey()) {
		userWantsReceipts_ = settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS);
		checkForDisplayingDisplayReceiptsAlert();
	}
}

void ChatController::checkForDisplayingDisplayReceiptsAlert() {
	if (userWantsReceipts_ && (contactSupportsReceipts_ == ChatWindow::No)) {
		chatWindow_->setAlert("This chat doesn't support delivery receipts.");
	} else if (userWantsReceipts_ && (contactSupportsReceipts_ == ChatWindow::Maybe)) {
		chatWindow_->setAlert("This chat may not support delivery receipts. You might not receive delivery receipts for the messages you sent.");
	} else {
		chatWindow_->cancelAlert();
	}
}

void ChatController::postSendMessage(const std::string& body, boost::shared_ptr<Stanza> sentStanza) {
	boost::shared_ptr<Replace> replace = sentStanza->getPayload<Replace>();
	if (replace) {
		eraseIf(unackedStanzas_, PairSecondEquals<boost::shared_ptr<Stanza>, std::string>(myLastMessageUIID_));
		chatWindow_->replaceMessage(body, myLastMessageUIID_, boost::posix_time::microsec_clock::universal_time());
	} else {
		myLastMessageUIID_ = addMessage(body, QT_TRANSLATE_NOOP("", "me"), true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel().getLabel() : boost::shared_ptr<SecurityLabel>(), std::string(avatarManager_->getAvatarPath(selfJID_).string()), boost::posix_time::microsec_clock::universal_time());
	}

	if (stanzaChannel_->getStreamManagementEnabled() && !myLastMessageUIID_.empty() ) {
		chatWindow_->setAckState(myLastMessageUIID_, ChatWindow::Pending);
		unackedStanzas_[sentStanza] = myLastMessageUIID_;
	}

	if (sentStanza->getPayload<DeliveryReceiptRequest>()) {
		requestedReceipts_[sentStanza->getID()] = myLastMessageUIID_;
		chatWindow_->setMessageReceiptState(myLastMessageUIID_, ChatWindow::ReceiptRequested);
	}

	lastWasPresence_ = false;
	chatStateNotifier_->userSentMessage();
}

void ChatController::handleStanzaAcked(boost::shared_ptr<Stanza> stanza) {
	std::map<boost::shared_ptr<Stanza>, std::string>::iterator unackedStanza = unackedStanzas_.find(stanza);
	if (unackedStanza != unackedStanzas_.end()) {
		chatWindow_->setAckState(unackedStanza->second, ChatWindow::Received);
		unackedStanzas_.erase(unackedStanza);
	}
}

void ChatController::setOnline(bool online) {
	if (!online) {
		std::map<boost::shared_ptr<Stanza>, std::string>::iterator it = unackedStanzas_.begin();
		for ( ; it != unackedStanzas_.end(); ++it) {
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

void ChatController::handleNewFileTransferController(FileTransferController* ftc) {
	std::string nick = senderDisplayNameFromMessage(ftc->getOtherParty());
	std::string ftID = ftc->setChatWindow(chatWindow_, nick);
	
	ftControllers[ftID] = ftc;
}

void ChatController::handleFileTransferCancel(std::string id) {
	std::cout << "handleFileTransferCancel(" << id << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->cancel();
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleFileTransferStart(std::string id, std::string description) {
	std::cout << "handleFileTransferStart(" << id << ", " << description << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->start(description);
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleFileTransferAccept(std::string id, std::string filename) {
	std::cout << "handleFileTransferAccept(" << id << ", " << filename << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->accept(filename);
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleSendFileRequest(std::string filename) {
	std::cout << "ChatController::handleSendFileRequest(" << filename << ")" << std::endl;
	eventStream_->send(boost::make_shared<SendFileUIEvent>(getToJID(), filename));
}

std::string ChatController::senderDisplayNameFromMessage(const JID& from) {
	return nickResolver_->jidToNick(from);
}

std::string ChatController::getStatusChangeString(boost::shared_ptr<Presence> presence) {
	std::string nick = senderDisplayNameFromMessage(presence->getFrom());
	std::string response;
	if (!presence || presence->getType() == Presence::Unavailable || presence->getType() == Presence::Error) {
		response = QT_TRANSLATE_NOOP("", "%1% has gone offline");
	} else if (presence->getType() == Presence::Available) {
		StatusShow::Type show = presence->getShow();
		if (show == StatusShow::Online || show == StatusShow::FFC) {
			response = QT_TRANSLATE_NOOP("", "%1% has become available");
		} else if (show == StatusShow::Away || show == StatusShow::XA) {
			response = QT_TRANSLATE_NOOP("", "%1% has gone away");
		} else if (show == StatusShow::DND) {
			response = QT_TRANSLATE_NOOP("", "%1% is now busy");
		} 
	}
	if (!response.empty()) {
		response = str(format(response) % nick);
	}

	if (!presence->getStatus().empty()) {
		response += " (" + presence->getStatus() + ")";
	}
	return response + ".";
}

void ChatController::handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
	bool me = false;
	if (toJID_.isBare()) {
		newPresence = presenceOracle_->getHighestPriorityPresence(toJID_);
		if ((newPresence ? newPresence->getShow() : StatusShow::None) != lastShownStatus_) {
			me = true;
		}
	} else if (toJID_.equals(newPresence->getFrom(), JID::WithResource)) {
		me = true;
	}
	if (!me) {
		return;
	}
	if (!newPresence) {
		newPresence = boost::make_shared<Presence>();
		newPresence->setType(Presence::Unavailable);
	}
	lastShownStatus_ = newPresence->getShow();

	chatStateTracker_->handlePresenceChange(newPresence);
	chatStateNotifier_->setContactIsOnline(newPresence->getType() == Presence::Available);
	std::string newStatusChangeString = getStatusChangeString(newPresence);
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
