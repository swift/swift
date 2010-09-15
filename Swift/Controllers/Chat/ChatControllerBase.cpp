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
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swiften/Queries/Requests/GetSecurityLabelsCatalogRequest.h"
#include "Swiften/Avatars/AvatarManager.h"

namespace Swift {

ChatControllerBase::ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory) : selfJID_(self), stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), chatWindowFactory_(chatWindowFactory), toJID_(toJID), labelsEnabled_(false), presenceOracle_(presenceOracle), avatarManager_(avatarManager), useDelayForLatency_(useDelayForLatency), eventController_(eventController), timerFactory_(timerFactory) {
	chatWindow_ = chatWindowFactory_->createChatWindow(toJID, eventStream);
	chatWindow_->onAllMessagesRead.connect(boost::bind(&ChatControllerBase::handleAllMessagesRead, this));
	chatWindow_->onSendMessageRequest.connect(boost::bind(&ChatControllerBase::handleSendMessageRequest, this, _1));
	setEnabled(stanzaChannel->isAvailable() && iqRouter->isAvailable());
	createDayChangeTimer();
}

ChatControllerBase::~ChatControllerBase() {
	delete chatWindow_;
}

void ChatControllerBase::createDayChangeTimer() {
	if (timerFactory_) {
		boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
		boost::posix_time::ptime midnight(now.date() + boost::gregorian::days(1));
		long millisecondsUntilMidnight = (midnight - now).total_milliseconds();
		dateChangeTimer_ = boost::shared_ptr<Timer>(timerFactory_->createTimer(millisecondsUntilMidnight));
		dateChangeTimer_->onTick.connect(boost::bind(&ChatControllerBase::handleDayChangeTick, this));
		dateChangeTimer_->start();
	}
}

void ChatControllerBase::handleDayChangeTick() {
	dateChangeTimer_->stop();
	boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
	chatWindow_->addSystemMessage("The day is now " + String(boost::posix_time::to_iso_extended_string(now)).getSubstring(0,10));
	createDayChangeTimer();
}

void ChatControllerBase::setEnabled(bool enabled) {
	chatWindow_->setInputEnabled(enabled);
}

void ChatControllerBase::setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info) {
	if (iqRouter_->isAvailable() && info->hasFeature(DiscoInfo::SecurityLabels)) {
		//chatWindow_->setSecurityLabelsEnabled(true);
		//chatWindow_->setSecurityLabelsError();
		boost::shared_ptr<GetSecurityLabelsCatalogRequest> request(new GetSecurityLabelsCatalogRequest(JID(toJID_.toBare()), iqRouter_));
		request->onResponse.connect(boost::bind(&ChatControllerBase::handleSecurityLabelsCatalogResponse, this, _1, _2));
		request->send();
		//labelsEnabled_ = true;
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
	if (useDelayForLatency_) {
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		message->addPayload(boost::shared_ptr<Delay>(new Delay(now, selfJID_)));
	}
	stanzaChannel_->sendMessage(message);
	postSendMessage(message->getBody(), boost::dynamic_pointer_cast<Stanza>(message));
}

void ChatControllerBase::handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog> catalog, const boost::optional<ErrorPayload>& error) {
	if (!error) {
		if (catalog->getLabels().size() == 0) {
			chatWindow_->setSecurityLabelsEnabled(false);
			labelsEnabled_ = false;
		} else {
			labelsEnabled_ = true;
			chatWindow_->setAvailableSecurityLabels(catalog->getLabels());
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

String ChatControllerBase::addMessage(const String& message, const String& senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath, const boost::posix_time::ptime& time) {
	if (message.beginsWith("/me ")) {
		return chatWindow_->addAction(message.getSplittedAtFirst(' ').second, senderName, senderIsSelf, label, avatarPath, time);
	} else {
		return chatWindow_->addMessage(message, senderName, senderIsSelf, label, avatarPath, time);
	}
}

bool ChatControllerBase::isFromContact(const JID& from) {
	return from.toBare() == toJID_.toBare();
}

void ChatControllerBase::handleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	if (messageEvent->isReadable()) {
		unreadMessages_.push_back(messageEvent);
	}



	preHandleIncomingMessage(messageEvent);	
	boost::shared_ptr<Message> message = messageEvent->getStanza();
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
		JID from = message->getFrom();
		std::vector<boost::shared_ptr<Delay> > delayPayloads = message->getPayloads<Delay>();
		for (size_t i = 0; useDelayForLatency_ && i < delayPayloads.size(); i++) {
			if (!delayPayloads[i]->getFrom()) { 
				continue;
			}
			boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
			std::ostringstream s;
			s << "The following message took " << (now - delayPayloads[i]->getStamp()).total_milliseconds() / 1000.0 <<  " seconds to be delivered from " << delayPayloads[i]->getFrom()->toString() << ".";
			chatWindow_->addSystemMessage(String(s.str()));
		}
		boost::shared_ptr<SecurityLabel> label = message->getPayload<SecurityLabel>();
		boost::optional<SecurityLabel> maybeLabel = label ? boost::optional<SecurityLabel>(*label) : boost::optional<SecurityLabel>();

		// Determine the timestamp
		boost::posix_time::ptime timeStamp = boost::posix_time::microsec_clock::universal_time();
		boost::optional<boost::posix_time::ptime> messageTimeStamp = getMessageTimestamp(message);
		if (messageTimeStamp) {
			timeStamp = *messageTimeStamp;
		}

		addMessage(body, senderDisplayNameFromMessage(from), isIncomingMessageFromMe(message), maybeLabel, String(avatarManager_->getAvatarPath(from).string()), timeStamp);
	}
	chatWindow_->show();
	chatWindow_->setUnreadMessageCount(unreadMessages_.size());
}

String ChatControllerBase::getErrorMessage(boost::shared_ptr<ErrorPayload> error) {
	String defaultMessage = "Error sending message";
	if (!error->getText().isEmpty()) {
		return error->getText();
	}
	else {
		switch (error->getCondition()) {
			case ErrorPayload::BadRequest: return "Bad request"; break;
			case ErrorPayload::Conflict: return "Conflict"; break;
			case ErrorPayload::FeatureNotImplemented: return "This feature is not implemented"; break;
			case ErrorPayload::Forbidden: return "Forbidden"; break;
			case ErrorPayload::Gone: return "Recipient can no longer be contacted"; break;
			case ErrorPayload::InternalServerError: return "Internal server error"; break;
			case ErrorPayload::ItemNotFound: return "Item not found"; break;
			case ErrorPayload::JIDMalformed: return "JID Malformed"; break;
			case ErrorPayload::NotAcceptable: return "Message was rejected"; break;
			case ErrorPayload::NotAllowed: return "Not allowed"; break;
			case ErrorPayload::NotAuthorized: return "Not authorized"; break;
			case ErrorPayload::PaymentRequired: return "Payment is required"; break;
			case ErrorPayload::RecipientUnavailable: return "Recipient is unavailable."; break;
			case ErrorPayload::Redirect: return "Redirect"; break;
			case ErrorPayload::RegistrationRequired: return "Registration required"; break;
			case ErrorPayload::RemoteServerNotFound: return "Recipient's server not found."; break;
			case ErrorPayload::RemoteServerTimeout: return "Remote server timeout"; break;
			case ErrorPayload::ResourceConstraint: return "The server is low on resources"; break;
			case ErrorPayload::ServiceUnavailable: return "The service is unavailable"; break;
			case ErrorPayload::SubscriptionRequired: return "A subscription is required"; break;
			case ErrorPayload::UndefinedCondition: return "Undefined condition"; break;
			case ErrorPayload::UnexpectedRequest: return "Unexpected request"; break;
		}
	}
	return defaultMessage;
}

}
