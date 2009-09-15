#include "Swift/Controllers/ChatControllerBase.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Base/foreach.h"
#include "Swift/Controllers/ChatWindow.h"
#include "Swift/Controllers/ChatWindowFactory.h"
#include "Swiften/Queries/Requests/GetSecurityLabelsCatalogRequest.h"
#include "Swiften/Avatars/AvatarManager.h"

namespace Swift {

ChatControllerBase::ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager) : selfJID_(self), stanzaChannel_(stanzaChannel), iqRouter_(iqRouter), chatWindowFactory_(chatWindowFactory), toJID_(toJID), labelsEnabled_(false), presenceOracle_(presenceOracle), avatarManager_(avatarManager) {
	chatWindow_ = chatWindowFactory_->createChatWindow(toJID);
	chatWindow_->onAllMessagesRead.connect(boost::bind(&ChatControllerBase::handleAllMessagesRead, this));
	chatWindow_->onSendMessageRequest.connect(boost::bind(&ChatControllerBase::handleSendMessageRequest, this, _1));
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
	stanzaChannel_->sendMessage(message);
	postSendMessage(message->getBody());
}

void ChatControllerBase::handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog> catalog, const boost::optional<Error>& error) {
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

void ChatControllerBase::handleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent) {
	unreadMessages_.push_back(messageEvent);
	chatWindow_->setUnreadMessageCount(unreadMessages_.size());

	boost::shared_ptr<Message> message = messageEvent->getStanza();
	preHandleIncomingMessage(message);	
	String body = message->getBody();
	if (message->isError()) {
		String errorMessage = getErrorMessage(message->getPayload<Error>());
		chatWindow_->addErrorMessage(errorMessage);
	}
	else {
		showChatWindow();
		boost::shared_ptr<SecurityLabel> label = message->getPayload<SecurityLabel>();
		boost::optional<SecurityLabel> maybeLabel = label ? boost::optional<SecurityLabel>(*label) : boost::optional<SecurityLabel>();
		JID from = message->getFrom();
		chatWindow_->addMessage(body, senderDisplayNameFromMessage(from), isIncomingMessageFromMe(message), maybeLabel, String(avatarManager_->getAvatarPath(from).string()));
	}
}

String ChatControllerBase::getErrorMessage(boost::shared_ptr<Error> error) {
	String defaultMessage = "Error sending message";
	if (!error->getText().isEmpty()) {
		return error->getText();
	}
	else {
		switch (error->getCondition()) {
			case Error::BadRequest: return defaultMessage; break;
			case Error::Conflict: return defaultMessage; break;
			case Error::FeatureNotImplemented: return defaultMessage; break;
			case Error::Forbidden: return defaultMessage; break;
			case Error::Gone: return "Recipient can no longer be contacted"; break;
			case Error::InternalServerError: return "Internal server error"; break;
			case Error::ItemNotFound: return defaultMessage; break;
			case Error::JIDMalformed: return defaultMessage; break;
			case Error::NotAcceptable: return "Message was rejected"; break;
			case Error::NotAllowed: return defaultMessage; break;
			case Error::NotAuthorized: return defaultMessage; break;
			case Error::PaymentRequired: return defaultMessage; break;
			case Error::RecipientUnavailable: return "Recipient is unavailable."; break;
			case Error::Redirect: return defaultMessage; break;
			case Error::RegistrationRequired: return defaultMessage; break;
			case Error::RemoteServerNotFound: return "Recipient's server not found."; break;
			case Error::RemoteServerTimeout: return defaultMessage; break;
			case Error::ResourceConstraint: return defaultMessage; break;
			case Error::ServiceUnavailable: return defaultMessage; break;
			case Error::SubscriptionRequired: return defaultMessage; break;
			case Error::UndefinedCondition: return defaultMessage; break;
			case Error::UnexpectedRequest: return defaultMessage; break;
		}
	}
	return defaultMessage;
}

}
