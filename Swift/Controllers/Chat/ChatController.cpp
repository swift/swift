/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatController.h>

#include <stdio.h>

#include <boost/bind.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/format.h>
#include <Swiften/Chat/ChatStateNotifier.h>
#include <Swiften/Chat/ChatStateTracker.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Disco/FeatureOracle.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Elements/Idle.h>
#include <Swiften/FileTransfer/FileTransferManager.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/Highlighter.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/StatusUtil.h>
#include <Swift/Controllers/UIEvents/AcceptWhiteboardSessionUIEvent.h>
#include <Swift/Controllers/UIEvents/CancelWhiteboardSessionUIEvent.h>
#include <Swift/Controllers/UIEvents/InviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChangeBlockStateUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestInviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIEvents/ShowWhiteboardUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, SettingsProvider* settings, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, boost::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider)
	: ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager, useDelayForLatency, eventStream, eventController, timerFactory, entityCapsProvider, historyController, mucRegistry, highlightManager, chatMessageParser, autoAcceptMUCInviteDecider), eventStream_(eventStream), userWantsReceipts_(userWantsReceipts), settings_(settings), clientBlockListManager_(clientBlockListManager) {
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
	Idle::ref idle;
	if (theirPresence && (idle = theirPresence->getPayload<Idle>())) {
		startMessage += str(format(QT_TRANSLATE_NOOP("", ", who has been idle since %1%")) % dateTimeToLocalString(idle->getSince()));
	}
	startMessage += ": " + statusShowTypeToFriendlyName(theirPresence ? theirPresence->getShow() : StatusShow::None);
	if (theirPresence && !theirPresence->getStatus().empty()) {
		startMessage += " (" + theirPresence->getStatus() + ")";
	}
	lastShownStatus_ = theirPresence ? theirPresence->getShow() : StatusShow::None;
	chatStateNotifier_->setContactIsOnline(theirPresence && theirPresence->getType() == Presence::Available);
	startMessage += ".";
	chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(startMessage), ChatWindow::DefaultDirection);
	chatWindow_->onUserTyping.connect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
	chatWindow_->onUserCancelsTyping.connect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
	chatWindow_->onFileTransferStart.connect(boost::bind(&ChatController::handleFileTransferStart, this, _1, _2));
	chatWindow_->onFileTransferAccept.connect(boost::bind(&ChatController::handleFileTransferAccept, this, _1, _2));
	chatWindow_->onFileTransferCancel.connect(boost::bind(&ChatController::handleFileTransferCancel, this, _1));
	chatWindow_->onSendFileRequest.connect(boost::bind(&ChatController::handleSendFileRequest, this, _1));
	chatWindow_->onWhiteboardSessionAccept.connect(boost::bind(&ChatController::handleWhiteboardSessionAccept, this));
	chatWindow_->onWhiteboardSessionCancel.connect(boost::bind(&ChatController::handleWhiteboardSessionCancel, this));
	chatWindow_->onWhiteboardWindowShow.connect(boost::bind(&ChatController::handleWhiteboardWindowShow, this));
	chatWindow_->onBlockUserRequest.connect(boost::bind(&ChatController::handleBlockUserRequest, this));
	chatWindow_->onUnblockUserRequest.connect(boost::bind(&ChatController::handleUnblockUserRequest, this));
	chatWindow_->onInviteToChat.connect(boost::bind(&ChatController::handleInviteToChat, this, _1));
	chatWindow_->onClosed.connect(boost::bind(&ChatController::handleWindowClosed, this));
	handleBareJIDCapsChanged(toJID_);

	settings_->onSettingChanged.connect(boost::bind(&ChatController::handleSettingChanged, this, _1));
	eventStream_->onUIEvent.connect(boost::bind(&ChatController::handleUIEvent, this, _1));
}

void ChatController::handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/) {
	if (jid.toBare() == toJID_.toBare()) {
		chatWindow_->setName(nickResolver_->jidToNick(jid));
	}
}

ChatController::~ChatController() {
	eventStream_->onUIEvent.disconnect(boost::bind(&ChatController::handleUIEvent, this, _1));
	settings_->onSettingChanged.disconnect(boost::bind(&ChatController::handleSettingChanged, this, _1));
	nickResolver_->onNickChanged.disconnect(boost::bind(&ChatController::handleContactNickChanged, this, _1, _2));
	delete chatStateNotifier_;
	delete chatStateTracker_;
}

JID ChatController::getBaseJID() {
	return isInMUC_ ? toJID_ : ChatControllerBase::getBaseJID();
}

void ChatController::cancelReplaces() {
	lastWasPresence_ = false;
}

void ChatController::handleBareJIDCapsChanged(const JID& /*jid*/) {
	FeatureOracle featureOracle(entityCapsProvider_, presenceOracle_);

	chatWindow_->setCorrectionEnabled(featureOracle.isMessageCorrectionSupported(toJID_));
	chatWindow_->setFileTransferEnabled(featureOracle.isFileTransferSupported(toJID_));
	contactSupportsReceipts_ = featureOracle.isMessageReceiptsSupported(toJID_);

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

void ChatController::setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info) {
	ChatControllerBase::setAvailableServerFeatures(info);
	if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::BlockingCommandFeature)) {
		boost::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();

		blockingOnStateChangedConnection_ = blockList->onStateChanged.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));
		blockingOnItemAddedConnection_ = blockList->onItemAdded.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));
		blockingOnItemRemovedConnection_ = blockList->onItemRemoved.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));

		handleBlockingStateChanged();
	}
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
	chatStateNotifier_->receivedMessageFromContact(!!message->getPayload<ChatState>());

	// handle XEP-0184 Message Receipts
	// incomming receipts
	if (boost::shared_ptr<DeliveryReceipt> receipt = message->getPayload<DeliveryReceipt>()) {
		SWIFT_LOG(debug) << "received receipt for id: " << receipt->getReceivedID() << std::endl;
		if (requestedReceipts_.find(receipt->getReceivedID()) != requestedReceipts_.end()) {
			chatWindow_->setMessageReceiptState(requestedReceipts_[receipt->getReceivedID()], ChatWindow::ReceiptReceived);
			requestedReceipts_.erase(receipt->getReceivedID());
		}
	// incomming errors in response to send out receipts
	} else if (message->getPayload<DeliveryReceiptRequest>() && (message->getType() == Message::Error)) {
		if (requestedReceipts_.find(message->getID()) != requestedReceipts_.end()) {
			chatWindow_->setMessageReceiptState(requestedReceipts_[message->getID()], ChatWindow::ReceiptFailed);
			requestedReceipts_.erase(message->getID());
		}
	// incoming receipt requests
	} else if (message->getPayload<DeliveryReceiptRequest>()) {
		if (receivingPresenceFromUs_) {
			boost::shared_ptr<Message> receiptMessage = boost::make_shared<Message>();
			receiptMessage->setTo(toJID_);
			receiptMessage->addPayload(boost::make_shared<DeliveryReceipt>(message->getID()));
			stanzaChannel_->sendMessage(receiptMessage);
		}
	}
}

void ChatController::postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent, const HighlightAction& highlight) {
	eventController_->handleIncomingEvent(messageEvent);
	if (!messageEvent->getConcluded()) {
		highlighter_->handleHighlightAction(highlight);
	}
}


void ChatController::preSendMessageRequest(boost::shared_ptr<Message> message) {
	chatStateNotifier_->addChatStateRequest(message);
	if (userWantsReceipts_ && (contactSupportsReceipts_ != No) && message) {
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
	boost::optional<ChatWindow::AlertID> newDeliverReceiptAlert;
	if (userWantsReceipts_ && (contactSupportsReceipts_ == No)) {
		newDeliverReceiptAlert = chatWindow_->addAlert(QT_TRANSLATE_NOOP("", "This chat doesn't support delivery receipts."));
	} else if (userWantsReceipts_ && (contactSupportsReceipts_ == Maybe)) {
		newDeliverReceiptAlert = chatWindow_->addAlert(QT_TRANSLATE_NOOP("", "This chat may not support delivery receipts. You might not receive delivery receipts for the messages you send."));
	} else {
		if (deliveryReceiptAlert_) {
			chatWindow_->removeAlert(*deliveryReceiptAlert_);
			deliveryReceiptAlert_.reset();
		}
	}
	if (newDeliverReceiptAlert) {
		if (deliveryReceiptAlert_) {
			chatWindow_->removeAlert(*deliveryReceiptAlert_);
		}
		deliveryReceiptAlert_ = newDeliverReceiptAlert;
	}
}

void ChatController::handleBlockingStateChanged() {
	boost::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();
	if (blockList->getState() == BlockList::Available) {
		if (isInMUC_ ? blockList->isBlocked(toJID_) : blockList->isBlocked(toJID_.toBare())) {
			if (!blockedContactAlert_) {
				blockedContactAlert_ = chatWindow_->addAlert(QT_TRANSLATE_NOOP("", "You've currently blocked this contact. To continue your conversation you have to unblock the contact first."));
			}
			chatWindow_->setBlockingState(ChatWindow::IsBlocked);

			// disconnect typing events to prevent chat state notifciations to blocked contacts
			chatWindow_->onUserTyping.disconnect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
			chatWindow_->onUserCancelsTyping.disconnect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
		} else {
			if (blockedContactAlert_) {
				chatWindow_->removeAlert(*blockedContactAlert_);
				blockedContactAlert_.reset();
			}
			chatWindow_->setBlockingState(ChatWindow::IsUnblocked);

			chatWindow_->onUserTyping.connect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
			chatWindow_->onUserCancelsTyping.connect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
		}
	}
}

void ChatController::handleBlockUserRequest() {
	if (isInMUC_) {
		eventStream_->send(boost::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Blocked, toJID_));
	} else {
		eventStream_->send(boost::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Blocked, toJID_.toBare()));
	}
}

void ChatController::handleUnblockUserRequest() {
	if (isInMUC_) {
		eventStream_->send(boost::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, toJID_));
	} else {
		eventStream_->send(boost::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, toJID_.toBare()));
	}
}

void ChatController::handleInviteToChat(const std::vector<JID>& droppedJIDs) {
	boost::shared_ptr<UIEvent> event(new RequestInviteToMUCUIEvent(toJID_.toBare(), droppedJIDs, RequestInviteToMUCUIEvent::Impromptu));
	eventStream_->send(event);
}

void ChatController::handleWindowClosed() {
	onWindowClosed();
}

void ChatController::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<InviteToMUCUIEvent> inviteEvent = boost::dynamic_pointer_cast<InviteToMUCUIEvent>(event);
	if (inviteEvent && inviteEvent->getRoom() == toJID_.toBare()) {
		onConvertToMUC(detachChatWindow(), inviteEvent->getInvites(), inviteEvent->getReason());
	}
}


void ChatController::postSendMessage(const std::string& body, boost::shared_ptr<Stanza> sentStanza) {
	boost::shared_ptr<Replace> replace = sentStanza->getPayload<Replace>();
	if (replace) {
		eraseIf(unackedStanzas_, PairSecondEquals<boost::shared_ptr<Stanza>, std::string>(myLastMessageUIID_));
		replaceMessage(body, myLastMessageUIID_, true, boost::posix_time::microsec_clock::universal_time(), HighlightAction());
	} else {
		myLastMessageUIID_ = addMessage(body, QT_TRANSLATE_NOOP("", "me"), true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel().getLabel() : boost::shared_ptr<SecurityLabel>(), avatarManager_->getAvatarPath(selfJID_), boost::posix_time::microsec_clock::universal_time(), HighlightAction());
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

void ChatController::handleWhiteboardSessionRequest(bool senderIsSelf) {
	lastWbID_ = chatWindow_->addWhiteboardRequest(senderIsSelf);
}

void ChatController::handleWhiteboardStateChange(const ChatWindow::WhiteboardSessionState state) {
	chatWindow_->setWhiteboardSessionStatus(lastWbID_, state);
}

void ChatController::handleFileTransferCancel(std::string id) {
	SWIFT_LOG(debug) << "handleFileTransferCancel(" << id << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->cancel();
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleFileTransferStart(std::string id, std::string description) {
	SWIFT_LOG(debug) << "handleFileTransferStart(" << id << ", " << description << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->start(description);
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleFileTransferAccept(std::string id, std::string filename) {
	SWIFT_LOG(debug) << "handleFileTransferAccept(" << id << ", " << filename << ")" << std::endl;
	if (ftControllers.find(id) != ftControllers.end()) {
		ftControllers[id]->accept(filename);
	} else {
		std::cerr << "unknown file transfer UI id" << std::endl;
	}
}

void ChatController::handleSendFileRequest(std::string filename) {
	SWIFT_LOG(debug) << "ChatController::handleSendFileRequest(" << filename << ")" << std::endl;
	eventStream_->send(boost::make_shared<SendFileUIEvent>(getToJID(), filename));
}

void ChatController::handleWhiteboardSessionAccept() {
	eventStream_->send(boost::make_shared<AcceptWhiteboardSessionUIEvent>(toJID_));
}

void ChatController::handleWhiteboardSessionCancel() {
	eventStream_->send(boost::make_shared<CancelWhiteboardSessionUIEvent>(toJID_));
}

void ChatController::handleWhiteboardWindowShow() {
	eventStream_->send(boost::make_shared<ShowWhiteboardUIEvent>(toJID_));
}

std::string ChatController::senderHighlightNameFromMessage(const JID& from) {
	if (isInMUC_) {
		return nickResolver_->jidToNick(from);
	}
	else {
		return from.toBare().toString();
	}
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
	Idle::ref idle;
	if ((idle = presence->getPayload<Idle>())) {
		response += str(format(QT_TRANSLATE_NOOP("", " and has been idle since %1%")) % dateTimeToLocalString(idle->getSince()));
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
			chatWindow_->replaceLastMessage(chatMessageParser_->parseMessageBody(newStatusChangeString), ChatWindow::UpdateTimestamp);
		} else {
			chatWindow_->addPresenceMessage(chatMessageParser_->parseMessageBody(newStatusChangeString), ChatWindow::DefaultDirection);
		}
		lastStatusChangeString_ = newStatusChangeString;
		lastWasPresence_ = true;
	}
}

boost::optional<boost::posix_time::ptime> ChatController::getMessageTimestamp(boost::shared_ptr<Message> message) const {
	return message->getTimestamp();
}

void ChatController::logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool /* isIncoming */) {
	HistoryMessage::Type type;
	if (mucRegistry_->isMUC(fromJID.toBare()) || mucRegistry_->isMUC(toJID.toBare())) {
		type = HistoryMessage::PrivateMessage;
	}
	else {
		type = HistoryMessage::Chat;
	}

	if (historyController_) {
		historyController_->addMessage(message, fromJID, toJID, type, timeStamp);
	}
}

ChatWindow* ChatController::detachChatWindow() {
	chatWindow_->onUserTyping.disconnect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
	chatWindow_->onUserCancelsTyping.disconnect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
	return ChatControllerBase::detachChatWindow();
}

}
