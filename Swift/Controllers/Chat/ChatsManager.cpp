/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatsManager.h"

#include <boost/bind.hpp>

#include "Swift/Controllers/Chat/ChatController.h"
#include "Swift/Controllers/Chat/MUCSearchController.h"
#include "Swift/Controllers/XMPPEvents/EventController.h"
#include "Swift/Controllers/Chat/MUCController.h"
#include "Swift/Controllers/UIEvents/RequestChatUIEvent.h"
#include "Swift/Controllers/UIEvents/JoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h"
#include "Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h"
#include "Swift/Controllers/UIInterfaces/ChatListWindowFactory.h"
#include "Swift/Controllers/UIInterfaces/JoinMUCWindow.h"
#include "Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Client/NickResolver.h"
#include "Swiften/MUC/MUCManager.h"
#include "Swiften/Elements/ChatState.h"
#include "Swiften/MUC/MUCBookmarkManager.h"

namespace Swift {

typedef std::pair<JID, ChatController*> JIDChatControllerPair;
typedef std::pair<JID, MUCController*> JIDMUCControllerPair;

ChatsManager::ChatsManager(
		JID jid, StanzaChannel* stanzaChannel, 
		IQRouter* iqRouter, 
		EventController* eventController, 
		ChatWindowFactory* chatWindowFactory, 
		JoinMUCWindowFactory* joinMUCWindowFactory, 
		NickResolver* nickResolver, 
		PresenceOracle* presenceOracle, 
		PresenceSender* presenceSender, 
		UIEventStream* uiEventStream, 
		ChatListWindowFactory* chatListWindowFactory, 
		bool useDelayForLatency, 
		TimerFactory* timerFactory, 
		MUCRegistry* mucRegistry, 
		EntityCapsProvider* entityCapsProvider, 
		MUCManager* mucManager,
		MUCSearchWindowFactory* mucSearchWindowFactory,
		SettingsProvider* settings) : 
			jid_(jid), 
			joinMUCWindowFactory_(joinMUCWindowFactory), 
			useDelayForLatency_(useDelayForLatency), 
			mucRegistry_(mucRegistry), 
			entityCapsProvider_(entityCapsProvider), 
			mucManager(mucManager)  {
	timerFactory_ = timerFactory;
	eventController_ = eventController;
	stanzaChannel_ = stanzaChannel;
	iqRouter_ = iqRouter;
	chatWindowFactory_ = chatWindowFactory;
	nickResolver_ = nickResolver;
	presenceOracle_ = presenceOracle;
	avatarManager_ = NULL;
	serverDiscoInfo_ = boost::shared_ptr<DiscoInfo>(new DiscoInfo());
	presenceSender_ = presenceSender;
	uiEventStream_ = uiEventStream;
	mucBookmarkManager_ = NULL;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1));
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&ChatsManager::handleUIEvent, this, _1));
	chatListWindow_ = chatListWindowFactory->createChatListWindow(uiEventStream_);
	joinMUCWindow_ = NULL;
	mucSearchController_ = new MUCSearchController(jid_, mucSearchWindowFactory, iqRouter, settings);
	mucSearchController_->onMUCSelected.connect(boost::bind(&ChatsManager::handleMUCSelectedAfterSearch, this, _1));
	setupBookmarks();
}

ChatsManager::~ChatsManager() {
	delete joinMUCWindow_;
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}
	delete mucBookmarkManager_;
	delete mucSearchController_;
}

void ChatsManager::setupBookmarks() {
	if (!mucBookmarkManager_) {
		mucBookmarkManager_ = new MUCBookmarkManager(iqRouter_);
		mucBookmarkManager_->onBookmarksReady.connect(boost::bind(&ChatsManager::handleBookmarksReady, this));
		mucBookmarkManager_->onBookmarkAdded.connect(boost::bind(&ChatsManager::handleMUCBookmarkAdded, this, _1));
		mucBookmarkManager_->onBookmarkRemoved.connect(boost::bind(&ChatsManager::handleMUCBookmarkRemoved, this, _1));

		if (chatListWindow_) {
			chatListWindow_->setBookmarksEnabled(false);
			chatListWindow_->clear();
		}
	}
}

void ChatsManager::handleBookmarksReady() {
	if (chatListWindow_) {
		chatListWindow_->setBookmarksEnabled(true);
	}
}

void ChatsManager::handleMUCBookmarkAdded(const MUCBookmark& bookmark) {
	std::map<JID, MUCController*>::iterator it = mucControllers_.find(bookmark.getRoom());
	if (it == mucControllers_.end() && bookmark.getAutojoin()) {
		//FIXME: need vcard stuff here to get a nick
		handleJoinMUCRequest(bookmark.getRoom(), bookmark.getNick(), false);
	}
	chatListWindow_->addMUCBookmark(bookmark);
}

void ChatsManager::handleMUCBookmarkRemoved(const MUCBookmark& bookmark) {
	chatListWindow_->removeMUCBookmark(bookmark);
}

void ChatsManager::handleUserLeftMUC(MUCController* mucController) {
	std::map<JID, MUCController*>::iterator it;
	for (it = mucControllers_.begin(); it != mucControllers_.end(); it++) {
		if ((*it).second == mucController) {
			mucControllers_.erase(it);
			delete mucController;
			return;
		}
	}
}

void ChatsManager::handleUIEvent(boost::shared_ptr<UIEvent> event) {
	boost::shared_ptr<RequestChatUIEvent> chatEvent = boost::dynamic_pointer_cast<RequestChatUIEvent>(event);
	if (chatEvent) {
		handleChatRequest(chatEvent->getContact());
		return;
	}
	boost::shared_ptr<RemoveMUCBookmarkUIEvent> removeMUCBookmarkEvent = boost::dynamic_pointer_cast<RemoveMUCBookmarkUIEvent>(event);
	if (removeMUCBookmarkEvent) {
		mucBookmarkManager_->removeBookmark(removeMUCBookmarkEvent->getBookmark());
		return;
	}
	boost::shared_ptr<AddMUCBookmarkUIEvent> addMUCBookmarkEvent = boost::dynamic_pointer_cast<AddMUCBookmarkUIEvent>(event);
	if (addMUCBookmarkEvent) {
		mucBookmarkManager_->addBookmark(addMUCBookmarkEvent->getBookmark());
		return;
	}

	boost::shared_ptr<EditMUCBookmarkUIEvent> editMUCBookmarkEvent = boost::dynamic_pointer_cast<EditMUCBookmarkUIEvent>(event);
	if (editMUCBookmarkEvent) {
		mucBookmarkManager_->replaceBookmark(editMUCBookmarkEvent->getOldBookmark(), editMUCBookmarkEvent->getNewBookmark());
	}
	else if (JoinMUCUIEvent::ref joinEvent = boost::dynamic_pointer_cast<JoinMUCUIEvent>(event)) {
		handleJoinMUCRequest(joinEvent->getJID(), joinEvent->getNick(), false);
	}
	else if (boost::dynamic_pointer_cast<RequestJoinMUCUIEvent>(event)) {
		if (!joinMUCWindow_) {
			joinMUCWindow_ = joinMUCWindowFactory_->createJoinMUCWindow();
			joinMUCWindow_->onJoinMUC.connect(boost::bind(&ChatsManager::handleJoinMUCRequest, this, _1, _2, _3));
			joinMUCWindow_->onSearchMUC.connect(boost::bind(&ChatsManager::handleSearchMUCRequest, this));
		}
		joinMUCWindow_->setMUC("");
		joinMUCWindow_->setNick(nickResolver_->jidToNick(jid_));
		joinMUCWindow_->show();
	}
}

/**
 * If a resource goes offline, release bound chatdialog to that resource.
 */
void ChatsManager::handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
	if (mucRegistry_->isMUC(newPresence->getFrom().toBare())) return;
	//if (newPresence->getType() != Presence::Unavailable) return;
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

void ChatsManager::setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info) {
	serverDiscoInfo_ = info;
	foreach (JIDChatControllerPair pair, chatControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
	foreach (JIDMUCControllerPair pair, mucControllers_) {
		pair.second->setAvailableServerFeatures(info);
	}
}

/**
 * This is to be called on connect/disconnect.
 */ 
void ChatsManager::setOnline(bool enabled) {
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		controllerPair.second->setOnline(enabled);
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		controllerPair.second->setOnline(enabled);
		if (enabled) {
			controllerPair.second->rejoin();
		}
	}
	if (!enabled) {
		delete mucBookmarkManager_;
		mucBookmarkManager_ = NULL;
		chatListWindow_->setBookmarksEnabled(false);
	} else {
		setupBookmarks();
	}

}

void ChatsManager::handleChatRequest(const std::string &contact) {
	ChatController* controller = getChatControllerOrFindAnother(JID(contact));
	controller->activateChatWindow();
}

ChatController* ChatsManager::getChatControllerOrFindAnother(const JID &contact) {
	ChatController* controller = getChatControllerIfExists(contact);
	if (!controller && !mucRegistry_->isMUC(contact.toBare())) {
		foreach (JIDChatControllerPair pair, chatControllers_) {
			if (pair.first.toBare() == contact.toBare()) {
				controller = pair.second;
				break;
			}
		}
	}
	return controller ? controller : createNewChatController(contact);
}

ChatController* ChatsManager::createNewChatController(const JID& contact) {
	assert(chatControllers_.find(contact) == chatControllers_.end());
	ChatController* controller = new ChatController(jid_, stanzaChannel_, iqRouter_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_, mucRegistry_->isMUC(contact.toBare()), useDelayForLatency_, uiEventStream_, eventController_, timerFactory_, entityCapsProvider_);
	chatControllers_[contact] = controller;
	controller->setAvailableServerFeatures(serverDiscoInfo_);
	return controller;
}

ChatController* ChatsManager::getChatControllerOrCreate(const JID &contact) {
	ChatController* controller = getChatControllerIfExists(contact);
	return controller ? controller : createNewChatController(contact);
}

ChatController* ChatsManager::getChatControllerIfExists(const JID &contact) {
	if (chatControllers_.find(contact) == chatControllers_.end()) {
		if (mucRegistry_->isMUC(contact.toBare())) {
			return NULL;
		}
		//Need to look for an unbound window to bind first
		JID bare(contact.toBare());
		if (chatControllers_.find(bare) != chatControllers_.end()) {
			rebindControllerJID(bare, contact);
		} else {
			foreach (JIDChatControllerPair pair, chatControllers_) {
				if (pair.first.toBare() == contact.toBare()) {
					rebindControllerJID(pair.first, contact);
					return chatControllers_[contact];
				}
			}
			return NULL;
		}
	}
	return chatControllers_[contact];
}

void ChatsManager::rebindControllerJID(const JID& from, const JID& to) {
	chatControllers_[to] = chatControllers_[from];
	chatControllers_.erase(from);
	chatControllers_[to]->setToJID(to);
}

void ChatsManager::handleJoinMUCRequest(const JID &mucJID, const boost::optional<std::string>& nickMaybe, bool autoJoin) {
	if (autoJoin) {
		MUCBookmark bookmark(mucJID, mucJID.getNode());
		bookmark.setAutojoin(true);
		if (nickMaybe) {
			bookmark.setNick(*nickMaybe);
		}
		mucBookmarkManager_->addBookmark(bookmark);
	}

	std::map<JID, MUCController*>::iterator it = mucControllers_.find(mucJID);
	if (it != mucControllers_.end()) {
		it->second->rejoin();
	} else {
		std::string nick = nickMaybe ? nickMaybe.get() : jid_.getNode();
		MUC::ref muc = mucManager->createMUC(mucJID);
		MUCController* controller = new MUCController(jid_, muc, nick, stanzaChannel_, iqRouter_, chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory_, eventController_);
		mucControllers_[mucJID] = controller;
		controller->setAvailableServerFeatures(serverDiscoInfo_);
		controller->onUserLeft.connect(boost::bind(&ChatsManager::handleUserLeftMUC, this, controller));
	}
	mucControllers_[mucJID]->activateChatWindow();
}

void ChatsManager::handleSearchMUCRequest() {
	mucSearchController_->openSearchWindow();
}

void ChatsManager::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));
	if (!event->isReadable() && !message->getPayload<ChatState>() && message->getSubject().empty()) {
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
	getChatControllerOrCreate(jid)->handleIncomingMessage(event);
}

void ChatsManager::handleMUCSelectedAfterSearch(const JID& muc) {
	if (joinMUCWindow_) {
		joinMUCWindow_->setMUC(muc.toString());
	}
}


}
