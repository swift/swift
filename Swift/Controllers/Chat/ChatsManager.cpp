/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/Chat/ChatsManager.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/Chat/ChatController.h>
#include <Swift/Controllers/Chat/ChatControllerBase.h>
#include <Swift/Controllers/Chat/MUCSearchController.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/MUC/MUCBookmarkManager.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Elements/MUCInvitationPayload.h>

namespace Swift {

typedef std::pair<JID, ChatController*> JIDChatControllerPair;
typedef std::pair<JID, MUCController*> JIDMUCControllerPair;

#define RECENT_CHATS "recent_chats"

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
		ProfileSettingsProvider* settings,
		FileTransferOverview* ftOverview) :
			jid_(jid), 
			joinMUCWindowFactory_(joinMUCWindowFactory), 
			useDelayForLatency_(useDelayForLatency), 
			mucRegistry_(mucRegistry), 
			entityCapsProvider_(entityCapsProvider), 
			mucManager(mucManager),
			ftOverview_(ftOverview) {
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
	profileSettings_ = settings;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1));
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&ChatsManager::handleUIEvent, this, _1));

	chatListWindow_ = chatListWindowFactory->createChatListWindow(uiEventStream_);
	chatListWindow_->onMUCBookmarkActivated.connect(boost::bind(&ChatsManager::handleMUCBookmarkActivated, this, _1));
	chatListWindow_->onRecentActivated.connect(boost::bind(&ChatsManager::handleRecentActivated, this, _1));
	chatListWindow_->onClearRecentsRequested.connect(boost::bind(&ChatsManager::handleClearRecentsRequested, this));

	joinMUCWindow_ = NULL;
	mucSearchController_ = new MUCSearchController(jid_, mucSearchWindowFactory, iqRouter, settings);
	mucSearchController_->onMUCSelected.connect(boost::bind(&ChatsManager::handleMUCSelectedAfterSearch, this, _1));
	ftOverview_->onNewFileTransferController.connect(boost::bind(&ChatsManager::handleNewFileTransferController, this, _1));
	setupBookmarks();
	loadRecents();
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

void ChatsManager::saveRecents() {
	std::string recents;
	int i = 1;
	foreach (ChatListWindow::Chat chat, recentChats_) {
		std::vector<std::string> activity;
		boost::split(activity, chat.activity, boost::is_any_of("\t\n"));
		if (activity.empty()) {
			/* Work around Boost bug https://svn.boost.org/trac/boost/ticket/4751 */
			activity.push_back("");
		}
		std::string recent = chat.jid.toString() + "\t" + activity[0] + "\t" + (chat.isMUC ? "true" : "false") +  "\t" + chat.nick;
		recents += recent + "\n";
		if (i++ > 25) {
			break;
		}
	}
	profileSettings_->storeString(RECENT_CHATS, recents);
}

void ChatsManager::handleClearRecentsRequested() {
	recentChats_.clear();
	saveRecents();
	handleUnreadCountChanged(NULL);
}

void ChatsManager::loadRecents() {
	std::string recentsString(profileSettings_->getStringSetting(RECENT_CHATS));
	std::vector<std::string> recents;
	boost::split(recents, recentsString, boost::is_any_of("\n"));
	int i = 0;
	foreach (std::string recentString, recents) {
		if (i++ > 30) {
			break;
		}
		std::vector<std::string> recent;
		boost::split(recent, recentString, boost::is_any_of("\t"));
		if (recent.size() < 4) {
			continue;
		}
		JID jid(recent[0]);
		if (!jid.isValid()) {
			continue;
		}
		std::string activity(recent[1]);
		bool isMUC = recent[2] == "true";
		std::string nick(recent[3]);
		StatusShow::Type type = StatusShow::None;
		boost::filesystem::path path;
		if (isMUC) {
			if (mucControllers_.find(jid.toBare()) != mucControllers_.end()) {
				type = StatusShow::Online;
			}
		} else {
			if (avatarManager_) {
				path = avatarManager_->getAvatarPath(jid);
			}
			Presence::ref presence = presenceOracle_->getHighestPriorityPresence(jid.toBare());
			type = presence ? presence->getShow() : StatusShow::None;
		}

		ChatListWindow::Chat chat(jid, nickResolver_->jidToNick(jid), activity, 0, type, path, isMUC, nick);
		prependRecent(chat);
	}
	handleUnreadCountChanged(NULL);
}

void ChatsManager::setupBookmarks() {
	if (!mucBookmarkManager_) {
		mucBookmarkManager_ = new MUCBookmarkManager(iqRouter_);
		mucBookmarkManager_->onBookmarksReady.connect(boost::bind(&ChatsManager::handleBookmarksReady, this));
		mucBookmarkManager_->onBookmarkAdded.connect(boost::bind(&ChatsManager::handleMUCBookmarkAdded, this, _1));
		mucBookmarkManager_->onBookmarkRemoved.connect(boost::bind(&ChatsManager::handleMUCBookmarkRemoved, this, _1));

		if (chatListWindow_) {
			chatListWindow_->setBookmarksEnabled(false);
			chatListWindow_->clearBookmarks();
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

ChatListWindow::Chat ChatsManager::createChatListChatItem(const JID& jid, const std::string& activity) {
	int unreadCount = 0;
	if (mucRegistry_->isMUC(jid)) {
		MUCController* controller = mucControllers_[jid.toBare()];
		StatusShow::Type type = StatusShow::None;
		std::string nick = "";
		if (controller) {
			unreadCount = controller->getUnreadCount();
			if (controller->isJoined()) {
				type = StatusShow::Online;
			}
			nick = controller->getNick();
		}
		return ChatListWindow::Chat(jid, jid.toString(), activity, unreadCount, type, boost::filesystem::path(), true, nick);

	} else {
		ChatController* controller = getChatControllerIfExists(jid, false);
		if (controller) {
			unreadCount = controller->getUnreadCount();
		}
		JID bareishJID = mucRegistry_->isMUC(jid.toBare()) ? jid : jid.toBare();
		Presence::ref presence = presenceOracle_->getHighestPriorityPresence(bareishJID);
		StatusShow::Type type = presence ? presence->getShow() : StatusShow::None;
		boost::filesystem::path avatarPath = avatarManager_ ? avatarManager_->getAvatarPath(bareishJID) : boost::filesystem::path();
		return ChatListWindow::Chat(bareishJID, nickResolver_->jidToNick(bareishJID), activity, unreadCount, type, avatarPath, false);
	}
}

void ChatsManager::handleChatActivity(const JID& jid, const std::string& activity, bool isMUC) {
	if (mucRegistry_->isMUC(jid.toBare()) && !isMUC) {
		/* Don't include PMs in MUC rooms.*/
		return;
	}
	ChatListWindow::Chat chat = createChatListChatItem(jid, activity);
	/* FIXME: handle nick changes */
	appendRecent(chat);
	handleUnreadCountChanged(NULL);
	saveRecents();
}

void ChatsManager::handleUnreadCountChanged(ChatControllerBase* controller) {
	int unreadTotal = 0;
	bool controllerIsMUC = dynamic_cast<MUCController*>(controller);
	bool isPM = controller && !controllerIsMUC && mucRegistry_->isMUC(controller->getToJID().toBare());
	foreach (ChatListWindow::Chat& chatItem, recentChats_) {
		bool match = false;
		if (controller) {
			/* Matching MUC item */
			match |= chatItem.isMUC == controllerIsMUC && chatItem.jid.toBare() == controller->getToJID().toBare();
			/* Matching PM */
			match |= isPM &&  chatItem.jid == controller->getToJID();
			/* Matching non-PM */
			match |= !isPM && !controllerIsMUC && chatItem.jid.toBare() == controller->getToJID().toBare();
		}
		if (match) {
			chatItem.setUnreadCount(controller->getUnreadCount());
		}
		unreadTotal += chatItem.unreadCount;
	}
	chatListWindow_->setRecents(recentChats_);
	chatListWindow_->setUnreadCount(unreadTotal);
}

void ChatsManager::appendRecent(const ChatListWindow::Chat& chat) {
	recentChats_.erase(std::remove(recentChats_.begin(), recentChats_.end(), chat), recentChats_.end());
	recentChats_.push_front(chat);
}

void ChatsManager::prependRecent(const ChatListWindow::Chat& chat) {
	recentChats_.erase(std::remove(recentChats_.begin(), recentChats_.end(), chat), recentChats_.end());
	recentChats_.push_back(chat);
}

void ChatsManager::handleUserLeftMUC(MUCController* mucController) {
	std::map<JID, MUCController*>::iterator it;
	for (it = mucControllers_.begin(); it != mucControllers_.end(); ++it) {
		if ((*it).second == mucController) {
			foreach (ChatListWindow::Chat& chat, recentChats_) {
				if (chat.isMUC && chat.jid == (*it).first) {
					chat.statusType = StatusShow::None;
					chatListWindow_->setRecents(recentChats_);
					break;
				}
			}
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
		handleJoinMUCRequest(joinEvent->getJID(), joinEvent->getNick(), joinEvent->getShouldJoinAutomatically());
		mucControllers_[joinEvent->getJID()]->activateChatWindow();
	}
	else if (boost::shared_ptr<RequestJoinMUCUIEvent> joinEvent = boost::dynamic_pointer_cast<RequestJoinMUCUIEvent>(event)) {
		if (!joinMUCWindow_) {
			joinMUCWindow_ = joinMUCWindowFactory_->createJoinMUCWindow(uiEventStream_);
			joinMUCWindow_->onSearchMUC.connect(boost::bind(&ChatsManager::handleSearchMUCRequest, this));
		}
		joinMUCWindow_->setMUC(joinEvent->getRoom());
		joinMUCWindow_->setNick(nickResolver_->jidToNick(jid_));
		joinMUCWindow_->show();
	}
}

/**
 * If a resource goes offline, release bound chatdialog to that resource.
 */
void ChatsManager::handlePresenceChange(boost::shared_ptr<Presence> newPresence) {
	if (mucRegistry_->isMUC(newPresence->getFrom().toBare())) return;

	foreach (ChatListWindow::Chat& chat, recentChats_) {
		if (newPresence->getFrom().toBare() == chat.jid.toBare() && !chat.isMUC) {
			Presence::ref presence = presenceOracle_->getHighestPriorityPresence(chat.jid.toBare());
			chat.setStatusType(presence ? presence->getShow() : StatusShow::None);
			chatListWindow_->setRecents(recentChats_);
			break;
		}
	}

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
	if (avatarManager_) {
		avatarManager_->onAvatarChanged.disconnect(boost::bind(&ChatsManager::handleAvatarChanged, this, _1));
	}
	avatarManager_ = avatarManager;
	foreach (ChatListWindow::Chat& chat, recentChats_) {
		if (!chat.isMUC) {
			chat.setAvatarPath(avatarManager_->getAvatarPath(chat.jid));
		}
	}
	avatarManager_->onAvatarChanged.connect(boost::bind(&ChatsManager::handleAvatarChanged, this, _1));
}

void ChatsManager::handleAvatarChanged(const JID& jid) {
	foreach (ChatListWindow::Chat& chat, recentChats_) {
		if (!chat.isMUC && jid.toBare() == chat.jid.toBare()) {
			chat.setAvatarPath(avatarManager_->getAvatarPath(jid));
			break;
		}
	}
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
	controller->onActivity.connect(boost::bind(&ChatsManager::handleChatActivity, this, contact, _1, false));
	controller->onUnreadCountChanged.connect(boost::bind(&ChatsManager::handleUnreadCountChanged, this, controller));
	return controller;
}

ChatController* ChatsManager::getChatControllerOrCreate(const JID &contact) {
	ChatController* controller = getChatControllerIfExists(contact);
	return controller ? controller : createNewChatController(contact);
}

ChatController* ChatsManager::getChatControllerIfExists(const JID &contact, bool rebindIfNeeded) {
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
					if (rebindIfNeeded) {
						rebindControllerJID(pair.first, contact);
						return chatControllers_[contact];
					} else {
						return pair.second;
					}

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

void ChatsManager::handleJoinMUCRequest(const JID &mucJID, const boost::optional<std::string>& nickMaybe, bool addAutoJoin) {
	if (addAutoJoin) {
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
		MUCController* controller = new MUCController(jid_, muc, nick, stanzaChannel_, iqRouter_, chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory_, eventController_, entityCapsProvider_);
		mucControllers_[mucJID] = controller;
		controller->setAvailableServerFeatures(serverDiscoInfo_);
		controller->onUserLeft.connect(boost::bind(&ChatsManager::handleUserLeftMUC, this, controller));
		controller->onUserJoined.connect(boost::bind(&ChatsManager::handleChatActivity, this, mucJID.toBare(), "", true));
		controller->onActivity.connect(boost::bind(&ChatsManager::handleChatActivity, this, mucJID.toBare(), _1, true));
		controller->onUnreadCountChanged.connect(boost::bind(&ChatsManager::handleUnreadCountChanged, this, controller));
		handleChatActivity(mucJID.toBare(), "", true);
	}

	mucControllers_[mucJID]->showChatWindow();
}

void ChatsManager::handleSearchMUCRequest() {
	mucSearchController_->openSearchWindow();
}

void ChatsManager::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));
	bool isInvite = message->getPayload<MUCInvitationPayload>();
	if (!event->isReadable() && !message->getPayload<ChatState>() && !isInvite && !message->hasSubject()) {
		return;
	}

	// Try to deliver it to a MUC
	if (message->getType() == Message::Groupchat || message->getType() == Message::Error || (isInvite && message->getType() == Message::Normal)) {
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

void ChatsManager::handleMUCBookmarkActivated(const MUCBookmark& mucBookmark) {
	uiEventStream_->send(boost::make_shared<JoinMUCUIEvent>(mucBookmark.getRoom(), mucBookmark.getNick()));
}

void ChatsManager::handleNewFileTransferController(FileTransferController* ftc) {
	ChatController* chatController = getChatControllerOrCreate(ftc->getOtherParty());
	chatController->handleNewFileTransferController(ftc);
	chatController->activateChatWindow();
}

void ChatsManager::handleRecentActivated(const ChatListWindow::Chat& chat) {
	if (chat.isMUC) {
		uiEventStream_->send(boost::make_shared<JoinMUCUIEvent>(chat.jid, chat.nick));
	}
	else {
		uiEventStream_->send(boost::make_shared<RequestChatUIEvent>(chat.jid));
	}
}

}
