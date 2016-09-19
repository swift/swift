/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatControllerBase.h>

#include <map>
#include <memory>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Path.h>
#include <Swiften/Base/format.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Queries/Requests/GetSecurityLabelsCatalogRequest.h>

#include <Swift/Controllers/Chat/AutoAcceptMUCInviteDecider.h>
#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Highlighting/HighlightManager.h>
#include <Swift/Controllers/Highlighting/Highlighter.h>
#include <Swift/Controllers/Intl.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>

namespace Swift {

ChatControllerBase::ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, EntityCapsProvider* entityCapsProvider, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, std::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider) : selfJID_(self), stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), chatWindowFactory_(chatWindowFactory), toJID_(toJID), labelsEnabled_(false), presenceOracle_(presenceOracle), avatarManager_(avatarManager), useDelayForLatency_(useDelayForLatency), eventController_(eventController), entityCapsProvider_(entityCapsProvider), historyController_(historyController), mucRegistry_(mucRegistry), chatMessageParser_(chatMessageParser), autoAcceptMUCInviteDecider_(autoAcceptMUCInviteDecider), eventStream_(eventStream) {
    chatWindow_ = chatWindowFactory_->createChatWindow(toJID, eventStream);
    chatWindow_->onAllMessagesRead.connect(boost::bind(&ChatControllerBase::handleAllMessagesRead, this));
    chatWindow_->onSendMessageRequest.connect(boost::bind(&ChatControllerBase::handleSendMessageRequest, this, _1, _2));
    chatWindow_->onContinuationsBroken.connect(boost::bind(&ChatControllerBase::handleContinuationsBroken, this));
    entityCapsProvider_->onCapsChanged.connect(boost::bind(&ChatControllerBase::handleCapsChanged, this, _1));
    highlighter_ = highlightManager->createHighlighter(nickResolver);
    ChatControllerBase::setOnline(stanzaChannel->isAvailable() && iqRouter->isAvailable());
}

ChatControllerBase::~ChatControllerBase() {
    delete highlighter_;
    delete chatWindow_;
}

void ChatControllerBase::handleContinuationsBroken() {
    cancelReplaces();
}

ChatWindow* ChatControllerBase::detachChatWindow() {
    ChatWindow* chatWindow = chatWindow_;
    chatWindow_ = nullptr;
    return chatWindow;
}

void ChatControllerBase::handleCapsChanged(const JID& jid) {
    if (jid.compare(toJID_, JID::WithoutResource) == 0) {
        handleBareJIDCapsChanged(jid);
    }
}

void ChatControllerBase::setCanStartImpromptuChats(bool supportsImpromptu) {
    if (chatWindow_) {
        chatWindow_->setCanInitiateImpromptuChats(supportsImpromptu);
    }
}

void ChatControllerBase::setEnabled(bool enabled) {
    chatWindow_->setOnline(enabled);
    chatWindow_->setCanInitiateImpromptuChats(false);
}

void ChatControllerBase::setOnline(bool online) {
    setEnabled(online);
}

JID ChatControllerBase::getBaseJID() {
    return JID(toJID_.toBare());
}

void ChatControllerBase::setAvailableServerFeatures(std::shared_ptr<DiscoInfo> info) {
    if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::SecurityLabelsCatalogFeature)) {
        GetSecurityLabelsCatalogRequest::ref request = GetSecurityLabelsCatalogRequest::create(getBaseJID(), iqRouter_);
        request->onResponse.connect(boost::bind(&ChatControllerBase::handleSecurityLabelsCatalogResponse, this, _1, _2));
        request->send();
    } else {
        chatWindow_->setSecurityLabelsEnabled(false);
        labelsEnabled_ = false;
    }
}

void ChatControllerBase::handleAllMessagesRead() {
    if (!unreadMessages_.empty()) {
        targetedUnreadMessages_.clear();
        for (std::shared_ptr<StanzaEvent> stanzaEvent : unreadMessages_) {
            stanzaEvent->conclude();
        }
        unreadMessages_.clear();
        updateMessageCount();
    }
}

int ChatControllerBase::getUnreadCount() {
    return boost::numeric_cast<int>(targetedUnreadMessages_.size());
}

void ChatControllerBase::handleSendMessageRequest(const std::string &body, bool isCorrectionMessage) {
    if (!stanzaChannel_->isAvailable() || body.empty()) {
        return;
    }
    std::shared_ptr<Message> message(new Message());
    message->setTo(toJID_);
    message->setType(Swift::Message::Chat);
    message->setBody(body);
    if (labelsEnabled_) {
        if (!isCorrectionMessage) {
            lastLabel_ = chatWindow_->getSelectedSecurityLabel();
        }
        SecurityLabelsCatalog::Item labelItem = lastLabel_;
        if (labelItem.getLabel()) {
            message->addPayload(labelItem.getLabel());
        }
    }
    preSendMessageRequest(message);

    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    if (useDelayForLatency_) {
        message->addPayload(std::make_shared<Delay>(now, selfJID_));
    }
    if (isCorrectionMessage) {
        message->addPayload(std::shared_ptr<Replace> (new Replace(lastSentMessageStanzaID_)));
    }
    message->setID(lastSentMessageStanzaID_ = idGenerator_.generateID());
    stanzaChannel_->sendMessage(message);
    postSendMessage(message->getBody().get(), std::dynamic_pointer_cast<Stanza>(message));
    onActivity(message->getBody().get());

#ifdef SWIFT_EXPERIMENTAL_HISTORY
    logMessage(body, selfJID_, toJID_, now, false);
#endif
}

void ChatControllerBase::handleSecurityLabelsCatalogResponse(std::shared_ptr<SecurityLabelsCatalog> catalog, ErrorPayload::ref error) {
    if (catalog && !error) {
        if (catalog->getItems().size() == 0) {
            chatWindow_->setSecurityLabelsEnabled(false);
            labelsEnabled_ = false;
        } else {
            labelsEnabled_ = true;
            chatWindow_->setAvailableSecurityLabels(catalog->getItems());
            chatWindow_->setSecurityLabelsEnabled(true);
        }
    } else {
        labelsEnabled_ = false;
        chatWindow_->setSecurityLabelsError();
    }
}

void ChatControllerBase::showChatWindow() {
    chatWindow_->show();
}

void ChatControllerBase::activateChatWindow() {
    chatWindow_->activate();
}

bool ChatControllerBase::hasOpenWindow() const {
    return chatWindow_ && chatWindow_->isVisible();
}

ChatWindow::ChatMessage ChatControllerBase::buildChatWindowChatMessage(const std::string& message, const std::string& senderName, bool senderIsSelf) {
    ChatWindow::ChatMessage chatMessage;
    chatMessage = chatMessageParser_->parseMessageBody(message, senderName, senderIsSelf);
    return chatMessage;
}

void ChatControllerBase::updateMessageCount() {
    chatWindow_->setUnreadMessageCount(boost::numeric_cast<int>(unreadMessages_.size()));
    onUnreadCountChanged();
}

std::string ChatControllerBase::addMessage(const ChatWindow::ChatMessage& chatMessage, const std::string& senderName, bool senderIsSelf, const std::shared_ptr<SecurityLabel> label, const boost::filesystem::path& avatarPath, const boost::posix_time::ptime& time) {
    if (chatMessage.isMeCommand()) {
        return chatWindow_->addAction(chatMessage, senderName, senderIsSelf, label, pathToString(avatarPath), time);
    }
    else {
        return chatWindow_->addMessage(chatMessage, senderName, senderIsSelf, label, pathToString(avatarPath), time);
    }
}

void ChatControllerBase::replaceMessage(const ChatWindow::ChatMessage& chatMessage, const std::string& id, const boost::posix_time::ptime& time) {
    if (chatMessage.isMeCommand()) {
        chatWindow_->replaceWithAction(chatMessage, id, time);
    }
    else {
        chatWindow_->replaceMessage(chatMessage, id, time);
    }
}

bool ChatControllerBase::isFromContact(const JID& from) {
    return from.toBare() == toJID_.toBare();
}

void ChatControllerBase::handleIncomingMessage(std::shared_ptr<MessageEvent> messageEvent) {
    std::shared_ptr<Message> message = messageEvent->getStanza();
    if (shouldIgnoreMessage(message)) {
        return;
    }

    preHandleIncomingMessage(messageEvent);
    if (messageEvent->isReadable() && !messageEvent->getConcluded()) {
        unreadMessages_.push_back(messageEvent);
        if (messageEvent->targetsMe()) {
            targetedUnreadMessages_.push_back(messageEvent);
        }
    }

    ChatWindow::ChatMessage chatMessage;
    boost::optional<std::string> optionalBody = message->getBody();
    std::string body = optionalBody.get_value_or("");
    if (message->isError()) {
        if (!message->getTo().getResource().empty()) {
            std::string errorMessage;
            if (message->getPayload<Swift::ErrorPayload>()->getCondition() == ErrorPayload::ItemNotFound) {
                errorMessage = QT_TRANSLATE_NOOP("", "This user could not be found in the room.");
            }
            else {
                errorMessage = str(format(QT_TRANSLATE_NOOP("", "Couldn't send message: %1%")) % getErrorMessage(message->getPayload<ErrorPayload>()));
            }
            chatWindow_->addErrorMessage(chatMessageParser_->parseMessageBody(errorMessage));
        }
    }
    else if (messageEvent->getStanza()->getPayload<MUCInvitationPayload>()) {
        handleMUCInvitation(messageEvent->getStanza());
        return;
    }
    else if (messageEvent->getStanza()->getPayload<MUCUserPayload>() && messageEvent->getStanza()->getPayload<MUCUserPayload>()->getInvite()) {
        handleMediatedMUCInvitation(messageEvent->getStanza());
        return;
    }
    else {
        if (!messageEvent->isReadable()) {
            return;
        }
        showChatWindow();
        JID from = message->getFrom();
        std::vector<std::shared_ptr<Delay> > delayPayloads = message->getPayloads<Delay>();
        for (size_t i = 0; useDelayForLatency_ && i < delayPayloads.size(); i++) {
            if (!delayPayloads[i]->getFrom()) {
                continue;
            }
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
            std::ostringstream s;
            s << "The following message took " << (now - delayPayloads[i]->getStamp()).total_milliseconds() / 1000.0 <<  " seconds to be delivered from " << delayPayloads[i]->getFrom()->toString() << ".";
            chatWindow_->addSystemMessage(chatMessageParser_->parseMessageBody(std::string(s.str())), ChatWindow::DefaultDirection);
        }
        std::shared_ptr<SecurityLabel> label = message->getPayload<SecurityLabel>();

        // Determine the timestamp
        boost::posix_time::ptime timeStamp = boost::posix_time::microsec_clock::universal_time();
        boost::optional<boost::posix_time::ptime> messageTimeStamp = getMessageTimestamp(message);
        if (messageTimeStamp) {
            timeStamp = *messageTimeStamp;
        }
        onActivity(body);

        std::shared_ptr<Replace> replace = message->getPayload<Replace>();
        bool senderIsSelf = isIncomingMessageFromMe(message);
        chatMessage = buildChatWindowChatMessage(body, senderHighlightNameFromMessage(from), senderIsSelf);
        if (replace) {
            handleIncomingReplaceMessage(from, chatMessage, message->getID(), replace->getID(), senderIsSelf, label, timeStamp);
        }
        else {
            addMessageHandleIncomingMessage(from, chatMessage, message->getID(), senderIsSelf, label, timeStamp);
        }

        logMessage(body, from, selfJID_, timeStamp, true);
    }
    chatWindow_->show();
    updateMessageCount();
    postHandleIncomingMessage(messageEvent, chatMessage);
}

std::string ChatControllerBase::getErrorMessage(std::shared_ptr<ErrorPayload> error) {
    std::string defaultMessage = QT_TRANSLATE_NOOP("", "Error sending message");
    if (!error->getText().empty()) {
        return error->getText();
    }
    else {
        switch (error->getCondition()) {
            case ErrorPayload::BadRequest: return QT_TRANSLATE_NOOP("", "Bad request");
            case ErrorPayload::Conflict: return QT_TRANSLATE_NOOP("", "Conflict");
            case ErrorPayload::FeatureNotImplemented: return QT_TRANSLATE_NOOP("", "This feature is not implemented");
            case ErrorPayload::Forbidden: return QT_TRANSLATE_NOOP("", "Forbidden");
            case ErrorPayload::Gone: return QT_TRANSLATE_NOOP("", "Recipient can no longer be contacted");
            case ErrorPayload::InternalServerError: return QT_TRANSLATE_NOOP("", "Internal server error");
            case ErrorPayload::ItemNotFound: return QT_TRANSLATE_NOOP("", "Item not found");
            case ErrorPayload::JIDMalformed: return QT_TRANSLATE_NOOP("", "JID Malformed");
            case ErrorPayload::NotAcceptable: return QT_TRANSLATE_NOOP("", "Message was rejected");
            case ErrorPayload::NotAllowed: return QT_TRANSLATE_NOOP("", "Not allowed");
            case ErrorPayload::NotAuthorized: return QT_TRANSLATE_NOOP("", "Not authorized");
            case ErrorPayload::PaymentRequired: return QT_TRANSLATE_NOOP("", "Payment is required");
            case ErrorPayload::RecipientUnavailable: return QT_TRANSLATE_NOOP("", "Recipient is unavailable");
            case ErrorPayload::Redirect: return QT_TRANSLATE_NOOP("", "Redirect");
            case ErrorPayload::RegistrationRequired: return QT_TRANSLATE_NOOP("", "Registration required");
            case ErrorPayload::RemoteServerNotFound: return QT_TRANSLATE_NOOP("", "Recipient's server not found");
            case ErrorPayload::RemoteServerTimeout: return QT_TRANSLATE_NOOP("", "Remote server timeout");
            case ErrorPayload::ResourceConstraint: return QT_TRANSLATE_NOOP("", "The server is low on resources");
            case ErrorPayload::ServiceUnavailable: return QT_TRANSLATE_NOOP("", "The service is unavailable");
            case ErrorPayload::SubscriptionRequired: return QT_TRANSLATE_NOOP("", "A subscription is required");
            case ErrorPayload::UndefinedCondition: return QT_TRANSLATE_NOOP("", "Undefined condition");
            case ErrorPayload::UnexpectedRequest: return QT_TRANSLATE_NOOP("", "Unexpected request");
        }
    }
    assert(false);
    return defaultMessage;
}

void ChatControllerBase::handleGeneralMUCInvitation(MUCInviteEvent::ref event) {
    unreadMessages_.push_back(event);
    chatWindow_->show();
    updateMessageCount();
    chatWindow_->addMUCInvitation(senderDisplayNameFromMessage(event->getInviter()), event->getRoomJID(), event->getReason(), event->getPassword(), event->getDirect(), event->getImpromptu());
    eventController_->handleIncomingEvent(event);
    lastWasPresence_ = false;
}

void ChatControllerBase::handleMUCInvitation(Message::ref message) {
    MUCInvitationPayload::ref invite = message->getPayload<MUCInvitationPayload>();

    if (autoAcceptMUCInviteDecider_->isAutoAcceptedInvite(message->getFrom(), invite)) {
        eventStream_->send(std::make_shared<JoinMUCUIEvent>(invite->getJID(), boost::optional<std::string>(), boost::optional<std::string>(), false, false, true));
    } else {
        MUCInviteEvent::ref inviteEvent = std::make_shared<MUCInviteEvent>(toJID_, invite->getJID(), invite->getReason(), invite->getPassword(), true, invite->getIsImpromptu());
        handleGeneralMUCInvitation(inviteEvent);
    }
}

void ChatControllerBase::handleMediatedMUCInvitation(Message::ref message) {
    MUCUserPayload::Invite invite = *message->getPayload<MUCUserPayload>()->getInvite();
    JID from = message->getFrom();
    std::string reason;
    if (!invite.reason.empty()) {
        reason = invite.reason;
    }
    std::string password;
    if (message->getPayload<MUCUserPayload>()->getPassword()) {
        password = *message->getPayload<MUCUserPayload>()->getPassword();
    }

    MUCInviteEvent::ref inviteEvent = std::make_shared<MUCInviteEvent>(invite.from, from, reason, password, false, false);
    handleGeneralMUCInvitation(inviteEvent);
}

}
