#include "Swiften/Controllers/ChatController.h"

#include "Swiften/Controllers/ChatWindow.h"
#include "Swiften/Controllers/ChatWindowFactory.h"
#include "Swiften/Controllers/NickResolver.h"

namespace Swift {
	
/**
 * The controller does not gain ownership of the stanzaChannel, nor the factory.
 */
ChatController::ChatController(StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle)
 : ChatControllerBase(stanzaChannel, iqRouter, chatWindowFactory, contact, presenceOracle) {
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
	chatWindow_->addMessage(body, "me", true, labelsEnabled_ ? chatWindow_->getSelectedSecurityLabel() : boost::optional<SecurityLabel>());
}

String ChatController::senderDisplayNameFromMessage(JID from) {
	return nickResolver_->jidToNick(from);
}

}
