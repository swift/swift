#include "Swift/Controllers/Chat/ChatController.h"

#include <boost/bind.hpp>

#include "Swiften/Avatars/AvatarManager.h"
#include "Swift/Controllers/UIInterfaces/ChatWindow.h"
#include "Swift/Controllers/UIInterfaces/ChatWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager)
 : ChatControllerBase(self, stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager) {
	nickResolver_ = nickResolver;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatController::handlePresenceChange, this, _1, _2));
	chatWindow_->setName(nickResolver_->jidToNick(toJID_));
}

bool ChatController::isIncomingMessageFromMe(boost::shared_ptr<Message>) {
	return false;
}

void ChatController::preHandleIncomingMessage(boost::shared_ptr<Message> message) {
	JID from = message->getFrom();
	if (!from.equals(toJID_, JID::WithResource)) {
		if (toJID_.equals(from, JID::WithoutResource)  && toJID_.isBare()){
			toJID_ = from;
		}
	}
}

void ChatController::postSendMessage(const String& body) {
	addMessage(body, "me", true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel() : boost::optional<SecurityLabel>(), String(avatarManager_->getAvatarPath(selfJID_).string()));
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
	if (!(toJID_.isBare() && newPresence->getFrom().equals(toJID_, JID::WithoutResource)) && newPresence->getFrom() != toJID_) {
		return;
	}
	String newStatusChangeString = getStatusChangeString(newPresence);
	if (!previousPresence || newStatusChangeString != getStatusChangeString(previousPresence)) {
		chatWindow_->addSystemMessage(newStatusChangeString);
	}
}


}
