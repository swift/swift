#include "Swift/Controllers/ChatController.h"

#include "Swift/Controllers/ChatWindow.h"
#include "Swift/Controllers/ChatWindowFactory.h"
#include "Swift/Controllers/NickResolver.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager)
 : ChatControllerBase(stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle, avatarManager) {
	nickResolver_ = nickResolver;
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
	chatWindow_->addMessage(body, "me", true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel() : boost::optional<SecurityLabel>(), "");
}

String ChatController::senderDisplayNameFromMessage(const JID& from) {
	return nickResolver_->jidToNick(from);
}

}
