/*
 * Copyright (c) 2010-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatControllerBase.h"

#include <sstream>
#include <map>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string.hpp>

#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/format.h>
#include <Swiften/Base/String.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/UIInterfaces/ChatWindowFactory.h>
#include <Swiften/Queries/Requests/GetSecurityLabelsCatalogRequest.h>
#include <Swiften/Avatars/AvatarManager.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/HighlightManager.h>
#include <Swift/Controllers/Highlighter.h>

namespace Swift {

ChatControllerBase::ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager) : selfJID_(self), stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), chatWindowFactory_(chatWindowFactory), toJID_(toJID), labelsEnabled_(false), presenceOracle_(presenceOracle), avatarManager_(avatarManager), useDelayForLatency_(useDelayForLatency), eventController_(eventController), timerFactory_(timerFactory), entityCapsProvider_(entityCapsProvider), historyController_(historyController), mucRegistry_(mucRegistry) {
	chatWindow_ = chatWindowFactory_->createChatWindow(toJID, eventStream);
	chatWindow_->onAllMessagesRead.connect(boost::bind(&ChatControllerBase::handleAllMessagesRead, this));
	chatWindow_->onSendMessageRequest.connect(boost::bind(&ChatControllerBase::handleSendMessageRequest, this, _1, _2));
	chatWindow_->onLogCleared.connect(boost::bind(&ChatControllerBase::handleLogCleared, this));
	entityCapsProvider_->onCapsChanged.connect(boost::bind(&ChatControllerBase::handleCapsChanged, this, _1));
	highlighter_ = highlightManager->createHighlighter();
	setOnline(stanzaChannel->isAvailable() && iqRouter->isAvailable());
	createDayChangeTimer();
}

ChatControllerBase::~ChatControllerBase() {
	delete chatWindow_;
}

void ChatControllerBase::handleLogCleared() {
	cancelReplaces();
}

void ChatControllerBase::handleCapsChanged(const JID& jid) {
	if (jid.compare(toJID_, JID::WithoutResource) == 0) {
		handleBareJIDCapsChanged(jid);
	}
}

void ChatControllerBase::createDayChangeTimer() {
	if (timerFactory_) {
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		boost::posix_time::ptime midnight(now.date() + boost::gregorian::days(1));
		int millisecondsUntilMidnight = boost::numeric_cast<int>((midnight - now).total_milliseconds());
		dateChangeTimer_ = boost::shared_ptr<Timer>(timerFactory_->createTimer(millisecondsUntilMidnight));
		dateChangeTimer_->onTick.connect(boost::bind(&ChatControllerBase::handleDayChangeTick, this));
		dateChangeTimer_->start();
	}
}

void ChatControllerBase::handleDayChangeTick() {
	dateChangeTimer_->stop();
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	chatWindow_->addSystemMessage(str(format(QT_TRANSLATE_NOOP("", "The day is now %1%")) % std::string(boost::posix_time::to_iso_extended_string(now)).substr(0,10)), ChatWindow::DefaultDirection);
	dayTicked();
	createDayChangeTimer();
}

void ChatControllerBase::setEnabled(bool enabled) {
	chatWindow_->setInputEnabled(enabled);
}

void ChatControllerBase::setOnline(bool online) {
	setEnabled(online);
}

JID ChatControllerBase::getBaseJID() {
	return JID(toJID_.toBare());
}

void ChatControllerBase::setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info) {
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
		foreach (boost::shared_ptr<StanzaEvent> stanzaEvent, unreadMessages_) {
			stanzaEvent->conclude();
		}
		unreadMessages_.clear();
		chatWindow_->setUnreadMessageCount(0);
		onUnreadCountChanged();
	}
}

int ChatControllerBase::getUnreadCount() {
	return boost::numeric_cast<int>(targetedUnreadMessages_.size());
}

void ChatControllerBase::handleSendMessageRequest(const std::string &body, bool isCorrectionMessage) {
	if (!stanzaChannel_->isAvailable() || body.empty()) {
		return;
	}
	boost::shared_ptr<Message> message(new Message());
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
		message->addPayload(boost::make_shared<Delay>(now, selfJID_));
	}
	if (isCorrectionMessage) {
		message->addPayload(boost::shared_ptr<Replace> (new Replace(lastSentMessageStanzaID_)));
	}
	message->setID(lastSentMessageStanzaID_ = idGenerator_.generateID());
	stanzaChannel_->sendMessage(message);
	postSendMessage(message->getBody(), boost::dynamic_pointer_cast<Stanza>(message));
	onActivity(message->getBody());

#ifdef SWIFT_EXPERIMENTAL_HISTORY
	logMessage(body, selfJID_, toJID_, now, false);
#endif
}

void ChatControllerBase::handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog> catalog, ErrorPayload::ref error) {
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

std::string ChatControllerBase::addMessage(const std::string& message, const std::string& senderName, bool senderIsSelf, const boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	if (boost::starts_with(message, "/me ")) {
		return chatWindow_->addAction(String::getSplittedAtFirst(message, ' ').second, senderName, senderIsSelf, label, avatarPath, time, highlight);
	} else {
		return chatWindow_->addMessage(message, senderName, senderIsSelf, label, avatarPath, time, highlight);
	}
}

void ChatControllerBase::replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) {
	if (boost::starts_with(message, "/me ")) {
		chatWindow_->replaceWithAction(String::getSplittedAtFirst(message, ' ').second, id, time, highlight);
	} else {
		chatWindow_->replaceMessage(message, id, time, highlight);
	}
}

bool ChatControllerBase::isFromContact(const JID& from) {
	return from.toBare() == toJID_.toBare();
}

void ChatControllerBase::handleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	preHandleIncomingMessage(messageEvent);
	if (messageEvent->isReadable() && !messageEvent->getConcluded()) {
		unreadMessages_.push_back(messageEvent);
		if (messageEvent->targetsMe()) {
			targetedUnreadMessages_.push_back(messageEvent);
		}
	}
	boost::shared_ptr<Message> message = messageEvent->getStanza();
	std::string body = message->getBody();
	HighlightAction highlight;
	if (message->isError()) {
		if (!message->getTo().getResource().empty()) {
			std::string errorMessage = str(format(QT_TRANSLATE_NOOP("", "Couldn't send message: %1%")) % getErrorMessage(message->getPayload<ErrorPayload>()));
			chatWindow_->addErrorMessage(errorMessage);
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
		std::vector<boost::shared_ptr<Delay> > delayPayloads = message->getPayloads<Delay>();
		for (size_t i = 0; useDelayForLatency_ && i < delayPayloads.size(); i++) {
			if (!delayPayloads[i]->getFrom()) {
				continue;
			}
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
			std::ostringstream s;
			s << "The following message took " << (now - delayPayloads[i]->getStamp()).total_milliseconds() / 1000.0 <<  " seconds to be delivered from " << delayPayloads[i]->getFrom()->toString() << ".";
			chatWindow_->addSystemMessage(std::string(s.str()), ChatWindow::DefaultDirection);
		}
		boost::shared_ptr<SecurityLabel> label = message->getPayload<SecurityLabel>();

		// Determine the timestamp
		boost::posix_time::ptime timeStamp = boost::posix_time::microsec_clock::universal_time();
		boost::optional<boost::posix_time::ptime> messageTimeStamp = getMessageTimestamp(message);
		if (messageTimeStamp) {
			timeStamp = *messageTimeStamp;
		}
		onActivity(body);

		// Highlight
		if (!isIncomingMessageFromMe(message)) {
			 highlight = highlighter_->findAction(body, senderDisplayNameFromMessage(from));
		}

 		boost::shared_ptr<Replace> replace = message->getPayload<Replace>();
		if (replace) {
			std::string body = message->getBody();
			// Should check if the user has a previous message
			std::map<JID, std::string>::iterator lastMessage;
			lastMessage = lastMessagesUIID_.find(from);
			if (lastMessage != lastMessagesUIID_.end()) {
				replaceMessage(body, lastMessagesUIID_[from], timeStamp, highlight);
			}
		}
		else {
			lastMessagesUIID_[from] = addMessage(body, senderDisplayNameFromMessage(from), isIncomingMessageFromMe(message), label, std::string(avatarManager_->getAvatarPath(from).string()), timeStamp, highlight);
		}

		logMessage(body, from, selfJID_, timeStamp, true);
	}
	chatWindow_->show();
	chatWindow_->setUnreadMessageCount(boost::numeric_cast<int>(unreadMessages_.size()));
	onUnreadCountChanged();
	postHandleIncomingMessage(messageEvent, highlight);
}

std::string ChatControllerBase::getErrorMessage(boost::shared_ptr<ErrorPayload> error) {
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
	chatWindow_->setUnreadMessageCount(boost::numeric_cast<int>(unreadMessages_.size()));
	onUnreadCountChanged();
	chatWindow_->addMUCInvitation(senderDisplayNameFromMessage(event->getInviter()), event->getRoomJID(), event->getReason(), event->getPassword(), event->getDirect());
	eventController_->handleIncomingEvent(event);
}

void ChatControllerBase::handleMUCInvitation(Message::ref message) {
	MUCInvitationPayload::ref invite = message->getPayload<MUCInvitationPayload>();

	MUCInviteEvent::ref inviteEvent = boost::make_shared<MUCInviteEvent>(toJID_, invite->getJID(), invite->getReason(), invite->getPassword(), true);
	handleGeneralMUCInvitation(inviteEvent);
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

	MUCInviteEvent::ref inviteEvent = boost::make_shared<MUCInviteEvent>(invite.from, from, reason, password, false);
	handleGeneralMUCInvitation(inviteEvent);
}



}
