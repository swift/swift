/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatController.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/Path.h>
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
#include <Swiften/Network/TimerFactory.h>

#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/Highlighting/Highlighter.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/StatusUtil.h>
#include <Swift/Controllers/Translator.h>
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
#include <Swift/Controllers/XMPPEvents/IncomingFileTransferEvent.h>

namespace Swift {

/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, TimerFactory* timerFactory, EventController* eventController, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, SettingsProvider* settings, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, std::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider)
    : ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, nickResolver, presenceOracle, avatarManager, useDelayForLatency, eventStream, eventController, entityCapsProvider, historyController, mucRegistry, highlightManager, chatMessageParser, autoAcceptMUCInviteDecider), userWantsReceipts_(userWantsReceipts), settings_(settings), clientBlockListManager_(clientBlockListManager) {
    isInMUC_ = isInMUC;
    lastWasPresence_ = false;
    chatStateNotifier_ = new ChatStateNotifier(stanzaChannel, contact, entityCapsProvider, timerFactory, 20000);
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
        theirPresence = contact.isBare() ? presenceOracle->getAccountPresence(contact) : presenceOracle->getLastPresence(contact);
    }
    Idle::ref idle;
    if (theirPresence && (idle = theirPresence->getPayload<Idle>())) {
        startMessage += str(format(QT_TRANSLATE_NOOP("", ", who has been idle since %1%")) % Swift::Translator::getInstance()->ptimeToHumanReadableString(idle->getSince()));
    }
    startMessage += ": " + statusShowTypeToFriendlyName(theirPresence ? theirPresence->getShow() : StatusShow::None);
    if (theirPresence && !theirPresence->getStatus().empty()) {
        startMessage += " (" + theirPresence->getStatus() + ")";
    }
    lastShownStatus_ = theirPresence ? theirPresence->getShow() : StatusShow::None;
    chatStateNotifier_->setContactIsOnline(theirPresence && theirPresence->getType() == Presence::Available);
    startMessage += ".";
    chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(startMessage), ChatWindow::DefaultDirection);
    chatWindow_->onContinuationsBroken.connect([this, startMessage]() { chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(startMessage), ChatWindow::DefaultDirection); });
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
    ChatController::handleBareJIDCapsChanged(toJID_);

    settings_->onSettingChanged.connect(boost::bind(&ChatController::handleSettingChanged, this, _1));
    eventStream_->onUIEvent.connect(boost::bind(&ChatController::handleUIEvent, this, _1));
}

void ChatController::handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/) {
    if (jid.toBare() == toJID_.toBare()) {
        chatWindow_->setName(nickResolver_->jidToNick(toJID_));
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
    chatWindow_->setFileTransferEnabled(isInMUC_ ? No : featureOracle.isFileTransferSupported(toJID_));
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
        presence = jid.isBare() ? presenceOracle_->getAccountPresence(jid.toBare()) : presenceOracle_->getLastPresence(jid);
    }
    chatStateNotifier_->setContactIsOnline(presence && presence->getType() == Presence::Available);
    handleBareJIDCapsChanged(toJID_);
}

void ChatController::setAvailableServerFeatures(std::shared_ptr<DiscoInfo> info) {
    ChatControllerBase::setAvailableServerFeatures(info);
    if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::BlockingCommandFeature)) {
        std::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();

        blockingOnStateChangedConnection_ = blockList->onStateChanged.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));
        blockingOnItemAddedConnection_ = blockList->onItemAdded.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));
        blockingOnItemRemovedConnection_ = blockList->onItemRemoved.connect(boost::bind(&ChatController::handleBlockingStateChanged, this));

        handleBlockingStateChanged();
    }
}

bool ChatController::isIncomingMessageFromMe(std::shared_ptr<Message>) {
    return false;
}

void ChatController::preHandleIncomingMessage(std::shared_ptr<MessageEvent> messageEvent) {
    if (messageEvent->isReadable()) {
        chatWindow_->flash();
        lastWasPresence_ = false;
    }
    std::shared_ptr<Message> message = messageEvent->getStanza();
    JID from = message->getFrom();
    if (!from.equals(toJID_, JID::WithResource)) {
        if (toJID_.equals(from, JID::WithoutResource)  && toJID_.isBare()){
            // Bind controller to a full JID if message contains body text or is a typing chat state.
            ChatState::ref chatState = message->getPayload<ChatState>();
            if (!message->getBody().get_value_or("").empty() || (chatState && chatState->getChatState() == ChatState::Composing)) {
                setToJID(from);
            }
        }
    }
    chatStateTracker_->handleMessageReceived(message);
    chatStateNotifier_->receivedMessageFromContact(!!message->getPayload<ChatState>());

    // handle XEP-0184 Message Receipts
    // incomming receipts
    if (std::shared_ptr<DeliveryReceipt> receipt = message->getPayload<DeliveryReceipt>()) {
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
            std::shared_ptr<Message> receiptMessage = std::make_shared<Message>();
            receiptMessage->setTo(toJID_);
            receiptMessage->addPayload(std::make_shared<DeliveryReceipt>(message->getID()));
            stanzaChannel_->sendMessage(receiptMessage);
        }
    }
}

void ChatController::postHandleIncomingMessage(std::shared_ptr<MessageEvent> messageEvent, const ChatWindow::ChatMessage& chatMessage) {
    highlighter_->handleSystemNotifications(chatMessage, messageEvent);
    eventController_->handleIncomingEvent(messageEvent);
    if (!messageEvent->getConcluded()) {
        highlighter_->handleSoundNotifications(chatMessage);
    }
}


void ChatController::preSendMessageRequest(std::shared_ptr<Message> message) {
    chatStateNotifier_->addChatStateRequest(message);
    if (userWantsReceipts_ && (contactSupportsReceipts_ != No) && message) {
        message->addPayload(std::make_shared<DeliveryReceiptRequest>());
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
    std::shared_ptr<BlockList> blockList = clientBlockListManager_->getBlockList();
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
        eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Blocked, toJID_));
    } else {
        eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Blocked, toJID_.toBare()));
    }
}

void ChatController::handleUnblockUserRequest() {
    if (isInMUC_) {
        eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, toJID_));
    } else {
        eventStream_->send(std::make_shared<RequestChangeBlockStateUIEvent>(RequestChangeBlockStateUIEvent::Unblocked, toJID_.toBare()));
    }
}

void ChatController::handleInviteToChat(const std::vector<JID>& droppedJIDs) {
    std::shared_ptr<UIEvent> event(new RequestInviteToMUCUIEvent(getToJID(), droppedJIDs, RequestInviteToMUCUIEvent::Impromptu));
    eventStream_->send(event);
}

void ChatController::handleWindowClosed() {
    onWindowClosed();
}

void ChatController::handleUIEvent(std::shared_ptr<UIEvent> event) {
    std::shared_ptr<InviteToMUCUIEvent> inviteEvent = std::dynamic_pointer_cast<InviteToMUCUIEvent>(event);
    if (inviteEvent && inviteEvent->getOriginator() == getToJID()) {
        onConvertToMUC(detachChatWindow(), inviteEvent->getInvites(), inviteEvent->getReason());
    }
}

void ChatController::handleIncomingOwnMessage(std::shared_ptr<Message> message) {
    if (!message->getBody().get_value_or("").empty()) {
        postSendMessage(message->getBody().get_value_or(""), message);
        handleStanzaAcked(message);
    }
}

void ChatController::postSendMessage(const std::string& body, std::shared_ptr<Stanza> sentStanza) {
    std::shared_ptr<Replace> replace = sentStanza->getPayload<Replace>();
    if (replace) {
        eraseIf(unackedStanzas_, PairSecondEquals<std::shared_ptr<Stanza>, std::string>(myLastMessageUIID_));
        replaceMessage(chatMessageParser_->parseMessageBody(body, "", true), myLastMessageUIID_, boost::posix_time::microsec_clock::universal_time());
    } else {
        myLastMessageUIID_ = addMessage(chatMessageParser_->parseMessageBody(body, "", true), QT_TRANSLATE_NOOP("", "me"), true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel().getLabel() : std::shared_ptr<SecurityLabel>(), avatarManager_->getAvatarPath(selfJID_), boost::posix_time::microsec_clock::universal_time());
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

void ChatController::handleStanzaAcked(std::shared_ptr<Stanza> stanza) {
    std::map<std::shared_ptr<Stanza>, std::string>::iterator unackedStanza = unackedStanzas_.find(stanza);
    if (unackedStanza != unackedStanzas_.end()) {
        chatWindow_->setAckState(unackedStanza->second, ChatWindow::Received);
        unackedStanzas_.erase(unackedStanza);
    }
}

void ChatController::setOnline(bool online) {
    if (!online) {
        for (auto& stanzaIdPair : unackedStanzas_) {
            chatWindow_->setAckState(stanzaIdPair.second, ChatWindow::Failed);
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
    std::string avatarPath;
    if (ftc->isIncoming()) {
        avatarPath = pathToString(avatarManager_->getAvatarPath(ftc->getOtherParty()));
    }
    else {
        avatarPath = pathToString(avatarManager_->getAvatarPath(selfJID_));
    }

    std::string ftID = ftc->setChatWindow(chatWindow_, nick, avatarPath);
    ftControllers[ftID] = ftc;
    lastWasPresence_ = false;

    if (ftc->isIncoming()) {
        auto incomingFileTransferEvent = std::make_shared<IncomingFileTransferEvent>(ftc->getOtherParty());
        if (hasOpenWindow()) {
            incomingFileTransferEvent->conclude();
        }
        else {
            unreadMessages_.push_back(incomingFileTransferEvent);
            updateMessageCount();
        }
        eventController_->handleIncomingEvent(incomingFileTransferEvent);
    }
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
    eventStream_->send(std::make_shared<SendFileUIEvent>(getToJID(), filename));
}

void ChatController::handleWhiteboardSessionAccept() {
    eventStream_->send(std::make_shared<AcceptWhiteboardSessionUIEvent>(toJID_));
}

void ChatController::handleWhiteboardSessionCancel() {
    eventStream_->send(std::make_shared<CancelWhiteboardSessionUIEvent>(toJID_));
}

void ChatController::handleWhiteboardWindowShow() {
    eventStream_->send(std::make_shared<ShowWhiteboardUIEvent>(toJID_));
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

std::string ChatController::getStatusChangeString(std::shared_ptr<Presence> presence) {
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
        response += str(format(QT_TRANSLATE_NOOP("", " and has been idle since %1%")) % Swift::Translator::getInstance()->ptimeToHumanReadableString(idle->getSince()));
    }

    if (!response.empty()) {
        response = str(format(response) % nick);
    }

    if (!presence->getStatus().empty()) {
        response += " (" + presence->getStatus() + ")";
    }
    return response + ".";
}

void ChatController::handlePresenceChange(std::shared_ptr<Presence> newPresence) {
    bool relevantPresence = false;

    if (isInMUC_) {
        // For MUC participants we only have a single presence to choose one and
        // even for multi-session nicknames multiple resources are not distinguishable
        // to other participants.
        if (toJID_.equals(newPresence->getFrom(), JID::WithResource)) {
            relevantPresence = true;
        }
    }
    else {
        // For standard chats we retrieve the account presence from the PresenceOracle,
        // as there can be multiple presences to choose from.
        if (toJID_.equals(newPresence->getFrom(), JID::WithoutResource)) {
            // Presence matches ChatController JID.
            newPresence = presenceOracle_->getAccountPresence(toJID_);
            relevantPresence = true;
        }
    }

    if (!relevantPresence) {
        return;
    }

    if (!newPresence) {
        newPresence = std::make_shared<Presence>();
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

boost::optional<boost::posix_time::ptime> ChatController::getMessageTimestamp(std::shared_ptr<Message> message) const {
    return message->getTimestamp();
}

void ChatController::addMessageHandleIncomingMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& timeStamp) {
    lastMessagesIDs_[from.toBare()] = {messageID, addMessage(message, senderDisplayNameFromMessage(from), senderIsSelf, label, avatarManager_->getAvatarPath(from), timeStamp)};
}

void ChatController::handleIncomingReplaceMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, const std::string& idToReplace, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& timeStamp) {
    auto lastMessage = lastMessagesIDs_.find(from.toBare());
    if ((lastMessage != lastMessagesIDs_.end()) && (lastMessage->second.idInStream == idToReplace)) {
        replaceMessage(message, lastMessage->second.idInWindow, timeStamp);
    }
    else {
        addMessageHandleIncomingMessage(from, message, messageID, senderIsSelf, label, timeStamp);
    }
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

bool ChatController::shouldIgnoreMessage(std::shared_ptr<Message> message) {
    if (!message->getID().empty()) {
        if (message->getID() == lastHandledMessageID_) {
            return true;
        }
        else {
            lastHandledMessageID_ = message->getID();
        }
    }
    return false;
}

JID ChatController::messageCorrectionJID(const JID& fromJID) {
    return fromJID.toBare();
}

ChatWindow* ChatController::detachChatWindow() {
    chatWindow_->onUserTyping.disconnect(boost::bind(&ChatStateNotifier::setUserIsTyping, chatStateNotifier_));
    chatWindow_->onUserCancelsTyping.disconnect(boost::bind(&ChatStateNotifier::userCancelledNewMessage, chatStateNotifier_));
    return ChatControllerBase::detachChatWindow();
}

}
