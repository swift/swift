#include "Swift/Controllers/Chat/ChatsManager.h"

#include <boost/bind.hpp>

#include "Swiften/Client/Client.h"

#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/EventController.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Elements/ChatState.h"

namespace Swift {

typedef std::pair<JID, ChatController*> JIDChatControllerPair;
typedef std::pair<JID, MUCController*> JIDMUCControllerPair;

ChatsManager::ChatsManager(JID jid, StanzaChannel* stanzaChannel, IQRouter* iqRouter, EventController* eventController, ChatWindowFactory* chatWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, boost::shared_ptr<DiscoInfo> serverDiscoInfo, PresenceSender* presenceSender) : jid_(jid) {
	eventController_ = eventController;
	stanzaChannel_ = stanzaChannel;
	iqRouter_ = iqRouter;
	chatWindowFactory_ = chatWindowFactory;
	treeWidgetFactory_ = treeWidgetFactory;
	nickResolver_ = nickResolver;
	presenceOracle_ = presenceOracle;
	avatarManager_ = NULL;
	serverDiscoInfo_ = serverDiscoInfo;
	presenceSender_ = presenceSender;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1, _2));
}

ChatsManager::~ChatsManager() {
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}

}

/**
 * If a resource goes offline, release bound chatdialog to that resource.
 */
void ChatsManager::handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> /*lastPresence*/) {
	if (newPresence->getType() != Presence::Unavailable) return;
	JID fullJID(newPresence->getFrom());
	std::map<JID, ChatController*>::iterator it = chatControllers_.find(fullJID);
	if (it == chatControllers_.end()) return;
	JID bareJID(fullJID.toBare());
	//It doesn't make sense to have two unbound dialogs.
	if (chatControllers_.find(bareJID) != chatControllers_.end()) return;
	rebindControllerJID(fullJID, bareJID);
}

void ChatsManager::setAvatarManager(AvatarManager* avatarManager) {
	avatarManager_ = avatarManager;
}

// void ChatsManager::handleUIEvent(boost::shared_ptr<UIEvent> rawEvent) {
// 	{
// 		boost::shared_ptr<RequestChatUIEvent> event = boost::dynamic_pointer_cast<RequestChatUIEvent>(rawEvent);
// 		if (event != NULL) {
// 			handleChatRequest(event->getContact());
// 			return;
// 		}
// 	}
// 	{
// 		boost::shared_ptr<JoinMUCUIEvent> event = boost::dynamic_pointer_cast<JoinMUCUIEvent>(rawEvent);
// 		if (event != NULL) {
// 			handleJoinMUCRequest(event->getRoom(), event->getNick());
// 		}
// 	}
// }


void ChatsManager::setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info) {
	foreach (JIDChatControllerPair pair, chatControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
	foreach (JIDMUCControllerPair pair, mucControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
}

void ChatsManager::setEnabled(bool enabled) {
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		//printf("Setting enabled on %d to %d\n", controllerPair.second, enabled);
		controllerPair.second->setEnabled(enabled);
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		controllerPair.second->setEnabled(enabled);
	}

}

void ChatsManager::handleChatRequest(const String &contact) {
	ChatController* controller = getChatController(JID(contact));
	controller->showChatWindow();
	controller->activateChatWindow();
}

ChatController* ChatsManager::getChatController(const JID &contact) {
	if (chatControllers_.find(contact) == chatControllers_.end()) {
		//Need to look for an unboud window to bind first
		JID bare(contact.toBare());
		if (chatControllers_.find(bare) != chatControllers_.end()) {
			rebindControllerJID(bare, contact);
		} else {
			chatControllers_[contact] = new ChatController(jid_, stanzaChannel_, iqRouter_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_);
			chatControllers_[contact]->setAvailableServerFeatures(serverDiscoInfo_);
		}
	}
	return chatControllers_[contact];
}

void ChatsManager::rebindControllerJID(const JID& from, const JID& to) {
	chatControllers_[to] = chatControllers_[from];
	chatControllers_.erase(from);
	chatControllers_[to]->setToJID(to);
}

void ChatsManager::handleJoinMUCRequest(const JID &muc, const String &nick) {
	std::map<JID, MUCController*>::iterator it = mucControllers_.find(muc);
	if (it != mucControllers_.end()) {
		//FIXME: What's correct behaviour here?
	} else {
		mucControllers_[muc] = new MUCController(jid_, muc, nick, stanzaChannel_, presenceSender_, iqRouter_, chatWindowFactory_, treeWidgetFactory_, presenceOracle_, avatarManager_);
		mucControllers_[muc]->setAvailableServerFeatures(serverDiscoInfo_);
	}
	mucControllers_[muc]->activateChatWindow();
}

void ChatsManager::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));
	if (!event->isReadable() && !message->getPayload<ChatState>()) {
		return;
	}

	// Try to deliver it to a MUC
	if (message->getType() == Message::Groupchat || message->getType() == Message::Error) {
		std::map<JID, MUCController*>::iterator i = mucControllers_.find(jid.toBare());
		if (i != mucControllers_.end()) {
			i->second->handleIncomingMessage(event);
			return;
		}
		else if (message->getType() == Message::Groupchat) {
			//FIXME: Error handling - groupchat messages from an unknown muc.
			return;
		}
	}
	
	//if not a mucroom
	eventController_->handleIncomingEvent(event);
	getChatController(jid)->handleIncomingMessage(event);
}

bool ChatsManager::isMUC(const JID& jid) const {
	return mucControllers_.find(jid.toBare()) != mucControllers_.end();
}



}
