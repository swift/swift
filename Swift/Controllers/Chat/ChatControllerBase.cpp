/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatControllerBase.h"

#include <sstream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Elements/Delay.h"
#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/Queries/Requests/GetSecurityLabelsCatalogRequest.h"
#include "Swiften/Avatars/AvatarManager.h"

namespace Swift {

ChatControllerBase::ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager) : selfJID_(self), stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), chatWindowFactory_(chatWindowFactory), toJID_(toJID), labelsEnabled_(false), presenceOracle_(presenceOracle), avatarManager_(avatarManager) {
	chatWindow_ = chatWindowFactory_->createChatWindow(toJID);
	chatWindow_->onAllMessagesRead.connect(boost::bind(&ChatControllerBase::handleAllMessagesRead, this));
	chatWindow_->onSendMessageRequest.connect(boost::bind(&ChatControllerBase::handleSendMessageRequest, this, _1));
	setEnabled(stanzaChannel->isAvailable() && iqRouter->isAvailable());
}

ChatControllerBase::~ChatControllerBase() {
	delete chatWindow_;
}

void ChatControllerBase::setEnabled(bool enabled) {
	chatWindow_->setInputEnabled(enabled);
}

void ChatControllerBase::setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info) {
	if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::SecurityLabels)) {
		chatWindow_->setSecurityLabelsEnabled(true);
		chatWindow_->setSecurityLabelsError();
		boost::shared_ptr<GetSecurityLabelsCatalogRequest> request(new GetSecurityLabelsCatalogRequest(JID(toJID_.toBare()), iqRouter_));
		request->onResponse.connect(boost::bind(&ChatControllerBase::handleSecurityLabelsCatalogResponse, this, _1, _2));
		request->send();
		labelsEnabled_ = true;
	} else {
		chatWindow_->setSecurityLabelsEnabled(false);
		labelsEnabled_ = false;
	}
}

void ChatControllerBase::handleAllMessagesRead() {
	foreach (boost::shared_ptr<MessageEvent> messageEvent, unreadMessages_) {
		messageEvent->read();
	}
	unreadMessages_.clear();
	chatWindow_->setUnreadMessageCount(0);
}

void ChatControllerBase::handleSendMessageRequest(const String &body) {
	if (!stanzaChannel_->isAvailable() || body.isEmpty()) {
		return;
	}
	boost::shared_ptr<Message> message(new Message());
	message->setTo(toJID_);
	message->setType(Swift::Message::Chat);
	message->setBody(body);
	boost::optional<SecurityLabel> label;
	if (labelsEnabled_) {
		message->addPayload(boost::shared_ptr<SecurityLabel>(new SecurityLabel(chatWindow_->getSelectedSecurityLabel())));
		label = boost::optional<SecurityLabel>(chatWindow_->getSelectedSecurityLabel());
	}
	preSendMessageRequest(message);
	//FIXME: optional
	bool useSwiftDelay = true;
	if (useSwiftDelay) {
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		message->addPayload(boost::shared_ptr<Delay>(new Delay(now, selfJID_)));
	}
	stanzaChannel_->sendMessage(message);
	postSendMessage(message->getBody());
}

void ChatControllerBase::handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog> catalog, const boost::optional<ErrorPayload>& error) {
	if (!error) {
		if (catalog->getLabels().size() == 0) {
			chatWindow_->setSecurityLabelsEnabled(false);
			labelsEnabled_ = false;
		} else {
			chatWindow_->setAvailableSecurityLabels(catalog->getLabels());
			chatWindow_->setSecurityLabelsEnabled(true);
		}
	} else {
		chatWindow_->setSecurityLabelsError();
	}
}

void ChatControllerBase::showChatWindow() {
	chatWindow_->show();
}

void ChatControllerBase::activateChatWindow() {
	chatWindow_->activate();
}

void ChatControllerBase::addMessage(const String& message, const String& senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) {
	if (message.beginsWith("/me ")) {
		chatWindow_->addAction(message.getSplittedAtFirst(' ').second, senderName, senderIsSelf, label, avatarPath);
	} else {
		chatWindow_->addMessage(message, senderName, senderIsSelf, label, avatarPath);
	}
}

void ChatControllerBase::handleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	if (messageEvent->isReadable()) {
		unreadMessages_.push_back(messageEvent);
	}
	chatWindow_->setUnreadMessageCount(unreadMessages_.size());

	boost::shared_ptr<Message> message = messageEvent->getStanza();
	preHandleIncomingMessage(message);	
	String body = message->getBody();
	if (message->isError()) {
		String errorMessage = getErrorMessage(message->getPayload<ErrorPayload>());
		chatWindow_->addErrorMessage(errorMessage);
	}
	else {
		if (!messageEvent->isReadable()) {
			return;
		}
		showChatWindow();
		boost::shared_ptr<Delay> delayPayload = message->getPayload<Delay>();
		if (delayPayload) {
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
			std::ostringstream s;
			s << "The following message took " << (now - delayPayload->getStamp()).total_milliseconds() <<  " milliseconds to be delivered.";
			chatWindow_->addSystemMessage(String(s.str()));
		}
		boost::shared_ptr<SecurityLabel> label = message->getPayload<SecurityLabel>();
		boost::optional<SecurityLabel> maybeLabel = label ? boost::optional<SecurityLabel>(*label) : boost::optional<SecurityLabel>();
		JID from = message->getFrom();
		addMessage(body, senderDisplayNameFromMessage(from), isIncomingMessageFromMe(message), maybeLabel, String(avatarManager_->getAvatarPath(from).string()));
	}
}

String ChatControllerBase::getErrorMessage(boost::shared_ptr<ErrorPayload> error) {
	String defaultMessage = "Error sending message";
	if (!error->getText().isEmpty()) {
		return error->getText();
	}
	else {
		switch (error->getCondition()) {
			case ErrorPayload::BadRequest: return defaultMessage; break;
			case ErrorPayload::Conflict: return defaultMessage; break;
			case ErrorPayload::FeatureNotImplemented: return defaultMessage; break;
			case ErrorPayload::Forbidden: return defaultMessage; break;
			case ErrorPayload::Gone: return "Recipient can no longer be contacted"; break;
			case ErrorPayload::InternalServerError: return "Internal server error"; break;
			case ErrorPayload::ItemNotFound: return defaultMessage; break;
			case ErrorPayload::JIDMalformed: return defaultMessage; break;
			case ErrorPayload::NotAcceptable: return "Message was rejected"; break;
			case ErrorPayload::NotAllowed: return defaultMessage; break;
			case ErrorPayload::NotAuthorized: return defaultMessage; break;
			case ErrorPayload::PaymentRequired: return defaultMessage; break;
			case ErrorPayload::RecipientUnavailable: return "Recipient is unavailable."; break;
			case ErrorPayload::Redirect: return defaultMessage; break;
			case ErrorPayload::RegistrationRequired: return defaultMessage; break;
			case ErrorPayload::RemoteServerNotFound: return "Recipient's server not found."; break;
			case ErrorPayload::RemoteServerTimeout: return defaultMessage; break;
			case ErrorPayload::ResourceConstraint: return defaultMessage; break;
			case ErrorPayload::ServiceUnavailable: return defaultMessage; break;
			case ErrorPayload::SubscriptionRequired: return defaultMessage; break;
			case ErrorPayload::UndefinedCondition: return defaultMessage; break;
			case ErrorPayload::UnexpectedRequest: return defaultMessage; break;
		}
	}
	return defaultMessage;
}

}
