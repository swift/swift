/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatsManager.h>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/DiscoServiceWalker.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/MUC/MUCBookmarkManager.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/VCards/VCardManager.h>

#include <Swift/Controllers/Chat/AutoAcceptMUCInviteDecider.h>
#include <Swift/Controllers/Chat/ChatController.h>
#include <Swift/Controllers/Chat/ChatControllerBase.h>
#include <Swift/Controllers/Chat/ChatMessageParser.h>
#include <Swift/Controllers/Chat/MUCController.h>
#include <Swift/Controllers/Chat/MUCSearchController.h>
#include <Swift/Controllers/Chat/UserSearchController.h>
#include <Swift/Controllers/FileTransfer/FileTransferController.h>
#include <Swift/Controllers/FileTransfer/FileTransferOverview.h>
#include <Swift/Controllers/ProfileSettingsProvider.h>
#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>
#include <Swift/Controllers/UIEvents/AddMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/CreateImpromptuMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/EditMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/InviteToMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/JoinMUCUIEvent.h>
#include <Swift/Controllers/UIEvents/RemoveMUCBookmarkUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/RequestJoinMUCUIEvent.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swift/Controllers/WhiteboardManager.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

BOOST_CLASS_VERSION(Swift::ChatListWindow::Chat, 1)

namespace boost {
namespace serialization {
	template<class Archive> void save(Archive& ar, const Swift::JID& jid, const unsigned int /*version*/) {
		std::string jidStr = jid.toString();
		ar << jidStr;
	}

	template<class Archive> void load(Archive& ar, Swift::JID& jid, const unsigned int /*version*/) {
		std::string stringJID;
		ar >> stringJID;
		jid = Swift::JID(stringJID);
	}

	template<class Archive> inline void serialize(Archive& ar, Swift::JID& t, const unsigned int file_version){
		split_free(ar, t, file_version);
	}

	template<class Archive> void serialize(Archive& ar, Swift::ChatListWindow::Chat& chat, const unsigned int version) {
		ar & chat.jid;
		ar & chat.chatName;
		ar & chat.activity;
		ar & chat.isMUC;
		ar & chat.nick;
		ar & chat.impromptuJIDs;
		if (version > 0) {
			ar & chat.password;
		}
	}
}
}

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
		ProfileSettingsProvider* profileSettings,
		FileTransferOverview* ftOverview,
		XMPPRoster* roster,
		bool eagleMode,
		SettingsProvider* settings,
		HistoryController* historyController,
		WhiteboardManager* whiteboardManager,
		HighlightManager* highlightManager,
		ClientBlockListManager* clientBlockListManager,
		const std::map<std::string, std::string>& emoticons,
		UserSearchController* inviteUserSearchController,
		VCardManager* vcardManager) :
			jid_(jid), 
			joinMUCWindowFactory_(joinMUCWindowFactory), 
			useDelayForLatency_(useDelayForLatency), 
			mucRegistry_(mucRegistry), 
			entityCapsProvider_(entityCapsProvider), 
			mucManager(mucManager),
			ftOverview_(ftOverview),
			roster_(roster),
			eagleMode_(eagleMode),
			settings_(settings),
			historyController_(historyController),
			whiteboardManager_(whiteboardManager),
			highlightManager_(highlightManager),
			emoticons_(emoticons),
			clientBlockListManager_(clientBlockListManager),
			inviteUserSearchController_(inviteUserSearchController),
			vcardManager_(vcardManager) {
	timerFactory_ = timerFactory;
	eventController_ = eventController;
	stanzaChannel_ = stanzaChannel;
	iqRouter_ = iqRouter;
	chatWindowFactory_ = chatWindowFactory;
	nickResolver_ = nickResolver;
	presenceOracle_ = presenceOracle;
	avatarManager_ = NULL;
	serverDiscoInfo_ = boost::make_shared<DiscoInfo>();
	presenceSender_ = presenceSender;
	uiEventStream_ = uiEventStream;
	mucBookmarkManager_ = NULL;
	profileSettings_ = profileSettings;
	presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1));
	uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&ChatsManager::handleUIEvent, this, _1));

	chatListWindow_ = chatListWindowFactory->createChatListWindow(uiEventStream_);
	chatListWindow_->onMUCBookmarkActivated.connect(boost::bind(&ChatsManager::handleMUCBookmarkActivated, this, _1));
	chatListWindow_->onRecentActivated.connect(boost::bind(&ChatsManager::handleRecentActivated, this, _1));
	chatListWindow_->onClearRecentsRequested.connect(boost::bind(&ChatsManager::handleClearRecentsRequested, this));

	joinMUCWindow_ = NULL;
	mucSearchController_ = new MUCSearchController(jid_, mucSearchWindowFactory, iqRouter, profileSettings_);
	mucSearchController_->onMUCSelected.connect(boost::bind(&ChatsManager::handleMUCSelectedAfterSearch, this, _1));
	ftOverview_->onNewFileTransferController.connect(boost::bind(&ChatsManager::handleNewFileTransferController, this, _1));
	whiteboardManager_->onSessionRequest.connect(boost::bind(&ChatsManager::handleWhiteboardSessionRequest, this, _1, _2));
	whiteboardManager_->onRequestAccepted.connect(boost::bind(&ChatsManager::handleWhiteboardStateChange, this, _1, ChatWindow::WhiteboardAccepted));
	whiteboardManager_->onSessionTerminate.connect(boost::bind(&ChatsManager::handleWhiteboardStateChange, this, _1, ChatWindow::WhiteboardTerminated));
	whiteboardManager_->onRequestRejected.connect(boost::bind(&ChatsManager::handleWhiteboardStateChange, this, _1, ChatWindow::WhiteboardRejected));
	roster_->onJIDAdded.connect(boost::bind(&ChatsManager::handleJIDAddedToRoster, this, _1));
	roster_->onJIDRemoved.connect(boost::bind(&ChatsManager::handleJIDRemovedFromRoster, this, _1));
	roster_->onJIDUpdated.connect(boost::bind(&ChatsManager::handleJIDUpdatedInRoster, this, _1));
	roster_->onRosterCleared.connect(boost::bind(&ChatsManager::handleRosterCleared, this));

	settings_->onSettingChanged.connect(boost::bind(&ChatsManager::handleSettingChanged, this, _1));

	userWantsReceipts_ = settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS);

	setupBookmarks();
	loadRecents();

	autoAcceptMUCInviteDecider_ = new AutoAcceptMUCInviteDecider(jid.getDomain(), roster_, settings_);
}

ChatsManager::~ChatsManager() {
	settings_->onSettingChanged.disconnect(boost::bind(&ChatsManager::handleSettingChanged, this, _1));
	roster_->onJIDAdded.disconnect(boost::bind(&ChatsManager::handleJIDAddedToRoster, this, _1));
	roster_->onJIDRemoved.disconnect(boost::bind(&ChatsManager::handleJIDRemovedFromRoster, this, _1));
	roster_->onJIDUpdated.disconnect(boost::bind(&ChatsManager::handleJIDUpdatedInRoster, this, _1));
	roster_->onRosterCleared.disconnect(boost::bind(&ChatsManager::handleRosterCleared, this));
	delete joinMUCWindow_;
	foreach (JIDChatControllerPair controllerPair, chatControllers_) {
		delete controllerPair.second;
	}
	foreach (JIDMUCControllerPair controllerPair, mucControllers_) {
		delete controllerPair.second;
	}
	delete mucBookmarkManager_;
	delete mucSearchController_;
	delete autoAcceptMUCInviteDecider_;
}

void ChatsManager::saveRecents() {
	std::stringstream serializeStream;
	boost::archive::text_oarchive oa(serializeStream);
	std::vector<ChatListWindow::Chat> recentsLimited = std::vector<ChatListWindow::Chat>(recentChats_.begin(), recentChats_.end());
	if (recentsLimited.size() > 25) {
		recentsLimited.erase(recentsLimited.begin() + 25, recentsLimited.end());
	}
	if (eagleMode_) {
		foreach(ChatListWindow::Chat& chat, recentsLimited) {
			chat.activity = "";
		}
	}

	class RemoveRecent {
	public:
		static bool ifPrivateMessage(const ChatListWindow::Chat& chat) {
			return chat.isPrivateMessage;
		}
	};

	recentsLimited.erase(std::remove_if(recentsLimited.begin(), recentsLimited.end(), RemoveRecent::ifPrivateMessage), recentsLimited.end());

	oa << recentsLimited;
	std::string serializedStr = Base64::encode(createByteArray(serializeStream.str()));
	profileSettings_->storeString(RECENT_CHATS, serializedStr);
}

void ChatsManager::handleClearRecentsRequested() {
	recentChats_.clear();
	saveRecents();
	handleUnreadCountChanged(NULL);
}

void ChatsManager::handleJIDAddedToRoster(const JID &jid) {
	updatePresenceReceivingStateOnChatController(jid);
}

void ChatsManager::handleJIDRemovedFromRoster(const JID &jid) {
	updatePresenceReceivingStateOnChatController(jid);
}

void ChatsManager::handleJIDUpdatedInRoster(const JID &jid) {
	updatePresenceReceivingStateOnChatController(jid);
}

void ChatsManager::handleRosterCleared() {
	/*	Setting that all chat controllers aren't receiving presence anymore;
		including MUC 1-to-1 chats due to the assumtion that this handler
		is only called on log out. */
	foreach(JIDChatControllerPair pair, chatControllers_) {
		pair.second->setContactIsReceivingPresence(false);
	}
}

void ChatsManager::updatePresenceReceivingStateOnChatController(const JID &jid) {
	ChatController* controller = getChatControllerIfExists(jid);
	if (controller) {
		if (!mucRegistry_->isMUC(jid.toBare())) {
			RosterItemPayload::Subscription subscription = roster_->getSubscriptionStateForJID(jid);
			controller->setContactIsReceivingPresence(subscription == RosterItemPayload::From || subscription == RosterItemPayload::Both);
		} else {
			controller->setContactIsReceivingPresence(true);
		}
	}
}

ChatListWindow::Chat ChatsManager::updateChatStatusAndAvatarHelper(const ChatListWindow::Chat& chat) const {
	ChatListWindow::Chat fixedChat = chat;
	if (fixedChat.isMUC) {
		if (mucControllers_.find(fixedChat.jid.toBare()) != mucControllers_.end()) {
			fixedChat.statusType = StatusShow::Online;
		}
	} else {
		if (avatarManager_) {
			fixedChat.avatarPath = avatarManager_->getAvatarPath(fixedChat.jid);
		}
		Presence::ref presence = presenceOracle_->getHighestPriorityPresence(fixedChat.jid.toBare());
		fixedChat.statusType = presence ? presence->getShow() : StatusShow::None;
	}
	return fixedChat;
}

void ChatsManager::loadRecents() {
	std::string recentsString(profileSettings_->getStringSetting(RECENT_CHATS));
	if (recentsString.find("\t") != std::string::npos) {
		// old format
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

			ChatListWindow::Chat chat(jid, nickResolver_->jidToNick(jid), activity, 0, type, path, isMUC, false, nick);
			chat = updateChatStatusAndAvatarHelper(chat);
			prependRecent(chat);
		}
	} else if (!recentsString.empty()){
		// boost searilaize based format
		ByteArray debase64 = Base64::decode(recentsString);
		std::vector<ChatListWindow::Chat> recentChats;
		std::stringstream deserializeStream(std::string(reinterpret_cast<const char*>(vecptr(debase64)), debase64.size()));
		try {
			boost::archive::text_iarchive ia(deserializeStream);
			ia >> recentChats;
		} catch (const boost::archive::archive_exception& e) {
			SWIFT_LOG(debug) << "Failed to load recents: " << e.what() << std::endl;
			return;
		}

		foreach(ChatListWindow::Chat chat, recentChats) {
			chat.statusType = StatusShow::None;
			chat = updateChatStatusAndAvatarHelper(chat);
			prependRecent(chat);
		}
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
		handleJoinMUCRequest(bookmark.getRoom(), bookmark.getPassword(), bookmark.getNick(), false, false, false  );
	}
	chatListWindow_->addMUCBookmark(bookmark);
}

void ChatsManager::handleMUCBookmarkRemoved(const MUCBookmark& bookmark) {
	chatListWindow_->removeMUCBookmark(bookmark);
}

ChatListWindow::Chat ChatsManager::createChatListChatItem(const JID& jid, const std::string& activity, bool privateMessage) {
	int unreadCount = 0;
	if (mucRegistry_->isMUC(jid)) {
		MUCController* controller = mucControllers_[jid.toBare()];
		StatusShow::Type type = StatusShow::None;
		std::string nick = "";
		std::string password = "";
		if (controller) {
			unreadCount = controller->getUnreadCount();
			if (controller->isJoined()) {
				type = StatusShow::Online;
			}
			nick = controller->getNick();

			if (controller->getPassword()) {
				password = *controller->getPassword();
			}

			if (controller->isImpromptu()) {
				ChatListWindow::Chat chat = ChatListWindow::Chat(jid, jid.toString(), activity, unreadCount, type, boost::filesystem::path(), true, privateMessage, nick, password);
				std::map<std::string, JID> participants = controller->getParticipantJIDs();
				chat.impromptuJIDs = participants;
				return chat;
			}
		}
		return ChatListWindow::Chat(jid, jid.toString(), activity, unreadCount, type, boost::filesystem::path(), true, privateMessage, nick, password);
	} else {
		ChatController* controller = getChatControllerIfExists(jid, false);
		if (controller) {
			unreadCount = controller->getUnreadCount();
		}
		JID bareishJID = mucRegistry_->isMUC(jid.toBare()) ? jid : jid.toBare();
		Presence::ref presence = presenceOracle_->getHighestPriorityPresence(bareishJID);
		StatusShow::Type type = presence ? presence->getShow() : StatusShow::None;
		boost::filesystem::path avatarPath = avatarManager_ ? avatarManager_->getAvatarPath(bareishJID) : boost::filesystem::path();
		return ChatListWindow::Chat(bareishJID, nickResolver_->jidToNick(bareishJID), activity, unreadCount, type, avatarPath, false, privateMessage);
	}
}

void ChatsManager::handleChatActivity(const JID& jid, const std::string& activity, bool isMUC) {
	const bool privateMessage = mucRegistry_->isMUC(jid.toBare()) && !isMUC;
	ChatListWindow::Chat chat = createChatListChatItem(jid, activity, privateMessage);
	/* FIXME: handle nick changes */
	appendRecent(chat);
	handleUnreadCountChanged(NULL);
	saveRecents();
}

void ChatsManager::handleChatClosed(const JID& /*jid*/) {
	cleanupPrivateMessageRecents();
	chatListWindow_->setRecents(recentChats_);
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

boost::optional<ChatListWindow::Chat> ChatsManager::removeExistingChat(const ChatListWindow::Chat& chat) {
	std::list<ChatListWindow::Chat>::iterator result = std::find(recentChats_.begin(), recentChats_.end(), chat);
	if (result != recentChats_.end()) {
		ChatListWindow::Chat existingChat = *result;
		recentChats_.erase(std::remove(recentChats_.begin(), recentChats_.end(), chat), recentChats_.end());
		return boost::optional<ChatListWindow::Chat>(existingChat);
	} else {
		return boost::optional<ChatListWindow::Chat>();
	}
}

void ChatsManager::cleanupPrivateMessageRecents() {
	/* if we leave a MUC and close a PM, remove it's recent chat entry */
	const std::list<ChatListWindow::Chat> chats = recentChats_;
	foreach (const ChatListWindow::Chat& chat, chats) {
		if (chat.isPrivateMessage) {
			typedef std::map<JID, MUCController*> ControllerMap;
			ControllerMap::iterator muc = mucControllers_.find(chat.jid.toBare());
			if (muc == mucControllers_.end() || !muc->second->isJoined()) {
				ChatController* chatController = getChatControllerIfExists(chat.jid);
				if (!chatController || !chatController->hasOpenWindow()) {
					removeExistingChat(chat);
					break;
				}
			}
		}
	}
}

void ChatsManager::appendRecent(const ChatListWindow::Chat& chat) {
	boost::optional<ChatListWindow::Chat> oldChat = removeExistingChat(chat);
	ChatListWindow::Chat mergedChat = chat;
	if (oldChat && !oldChat->impromptuJIDs.empty()) {
		mergedChat.impromptuJIDs.insert(oldChat->impromptuJIDs.begin(), oldChat->impromptuJIDs.end());
	}
	recentChats_.push_front(mergedChat);
}

void ChatsManager::prependRecent(const ChatListWindow::Chat& chat) {
	boost::optional<ChatListWindow::Chat> oldChat = removeExistingChat(chat);
	ChatListWindow::Chat mergedChat = chat;
	if (oldChat && !oldChat->impromptuJIDs.empty()) {
		mergedChat.impromptuJIDs.insert(oldChat->impromptuJIDs.begin(), oldChat->impromptuJIDs.end());
	}
	recentChats_.push_back(mergedChat);
}

void ChatsManager::handleUserLeftMUC(MUCController* mucController) {
	std::map<JID, MUCController*>::iterator it;
	for (it = mucControllers_.begin(); it != mucControllers_.end(); ++it) {
		if ((*it).second == mucController) {
			foreach (ChatListWindow::Chat& chat, recentChats_) {
				if (chat.isMUC && chat.jid == (*it).first) {
					chat.statusType = StatusShow::None;
				}
			}
			mucControllers_.erase(it);
			delete mucController;
			break;
		}
	}
	cleanupPrivateMessageRecents();
	chatListWindow_->setRecents(recentChats_);
}

void ChatsManager::handleSettingChanged(const std::string& settingPath) {
	if (settingPath == SettingConstants::REQUEST_DELIVERYRECEIPTS.getKey()) {
		userWantsReceipts_ = settings_->getSetting(SettingConstants::REQUEST_DELIVERYRECEIPTS);
		return;
	}
}

void ChatsManager::finalizeImpromptuJoin(MUC::ref muc, const std::vector<JID>& jidsToInvite, const std::string& reason, const boost::optional<JID>& reuseChatJID) {
	// send impromptu invites for the new MUC
	std::vector<JID> missingJIDsToInvite = jidsToInvite;

	typedef std::pair<std::string, MUCOccupant> StringMUCOccupantPair;
	std::map<std::string, MUCOccupant> occupants = muc->getOccupants();
	foreach(StringMUCOccupantPair occupant, occupants) {
		boost::optional<JID> realJID = occupant.second.getRealJID();
		if (realJID) {
			missingJIDsToInvite.erase(std::remove(missingJIDsToInvite.begin(), missingJIDsToInvite.end(), realJID->toBare()), missingJIDsToInvite.end());
		}
	}

	if (reuseChatJID) {
		muc->invitePerson(reuseChatJID.get(), reason, true, true);
	}
	foreach(const JID& jid, missingJIDsToInvite) {
		muc->invitePerson(jid, reason, true);
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

	boost::shared_ptr<CreateImpromptuMUCUIEvent> createImpromptuMUCEvent = boost::dynamic_pointer_cast<CreateImpromptuMUCUIEvent>(event);
	if (createImpromptuMUCEvent) {
		assert(!localMUCServiceJID_.toString().empty());
		// create new muc
		JID roomJID = createImpromptuMUCEvent->getRoomJID().toString().empty() ? JID(idGenerator_.generateID(), localMUCServiceJID_) : createImpromptuMUCEvent->getRoomJID();

		// join muc
		MUC::ref muc = handleJoinMUCRequest(roomJID, boost::optional<std::string>(), nickResolver_->jidToNick(jid_), false, true, true);
		mucControllers_[roomJID]->onImpromptuConfigCompleted.connect(boost::bind(&ChatsManager::finalizeImpromptuJoin, this, muc, createImpromptuMUCEvent->getJIDs(), createImpromptuMUCEvent->getReason(), boost::optional<JID>()));
		mucControllers_[roomJID]->activateChatWindow();
	}

	boost::shared_ptr<EditMUCBookmarkUIEvent> editMUCBookmarkEvent = boost::dynamic_pointer_cast<EditMUCBookmarkUIEvent>(event);
	if (editMUCBookmarkEvent) {
		mucBookmarkManager_->replaceBookmark(editMUCBookmarkEvent->getOldBookmark(), editMUCBookmarkEvent->getNewBookmark());
	}
	else if (JoinMUCUIEvent::ref joinEvent = boost::dynamic_pointer_cast<JoinMUCUIEvent>(event)) {
		handleJoinMUCRequest(joinEvent->getJID(), joinEvent->getPassword(), joinEvent->getNick(), joinEvent->getShouldJoinAutomatically(), joinEvent->getCreateAsReservedRoomIfNew(), joinEvent->isImpromptu());
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

void ChatsManager::markAllRecentsOffline() {
	foreach (ChatListWindow::Chat& chat, recentChats_) {
		chat.setStatusType(StatusShow::None);
	}

	chatListWindow_->setRecents(recentChats_);
}

void ChatsManager::handleTransformChatToMUC(ChatController* chatController, ChatWindow* chatWindow, const std::vector<JID>& jidsToInvite, const std::string& reason) {
	JID reuseChatInvite = chatController->getToJID();
	chatControllers_.erase(chatController->getToJID());
	delete chatController;

	// join new impromptu muc
	assert(!localMUCServiceJID_.toString().empty());

	// create new muc
	JID roomJID = JID(idGenerator_.generateID(), localMUCServiceJID_);

	// join muc
	MUC::ref muc = handleJoinMUCRequest(roomJID, boost::optional<std::string>(), nickResolver_->jidToNick(jid_), false, true, true, chatWindow);
	mucControllers_[roomJID]->onImpromptuConfigCompleted.connect(boost::bind(&ChatsManager::finalizeImpromptuJoin, this, muc, jidsToInvite, reason, boost::optional<JID>(reuseChatInvite)));
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
		markAllRecentsOffline();
	} else {
		setupBookmarks();
		localMUCServiceFinderWalker_ = boost::make_shared<DiscoServiceWalker>(jid_.getDomain(), iqRouter_);
		localMUCServiceFinderWalker_->onServiceFound.connect(boost::bind(&ChatsManager::handleLocalServiceFound, this, _1, _2));
		localMUCServiceFinderWalker_->onWalkAborted.connect(boost::bind(&ChatsManager::handleLocalServiceWalkFinished, this));
		localMUCServiceFinderWalker_->onWalkComplete.connect(boost::bind(&ChatsManager::handleLocalServiceWalkFinished, this));
		localMUCServiceFinderWalker_->beginWalk();
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
	boost::shared_ptr<ChatMessageParser> chatMessageParser = boost::make_shared<ChatMessageParser>(emoticons_, highlightManager_->getRules(), false); /* a message parser that knows this is a chat (not a room/MUC) */
	ChatController* controller = new ChatController(jid_, stanzaChannel_, iqRouter_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_, mucRegistry_->isMUC(contact.toBare()), useDelayForLatency_, uiEventStream_, eventController_, timerFactory_, entityCapsProvider_, userWantsReceipts_, settings_, historyController_, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser, autoAcceptMUCInviteDecider_);
	chatControllers_[contact] = controller;
	controller->setAvailableServerFeatures(serverDiscoInfo_);
	controller->onActivity.connect(boost::bind(&ChatsManager::handleChatActivity, this, contact, _1, false));
	controller->onWindowClosed.connect(boost::bind(&ChatsManager::handleChatClosed, this, contact));
	controller->onUnreadCountChanged.connect(boost::bind(&ChatsManager::handleUnreadCountChanged, this, controller));
	controller->onConvertToMUC.connect(boost::bind(&ChatsManager::handleTransformChatToMUC, this, controller, _1, _2, _3));
	updatePresenceReceivingStateOnChatController(contact);
	controller->setCanStartImpromptuChats(!localMUCServiceJID_.toString().empty());
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

MUC::ref ChatsManager::handleJoinMUCRequest(const JID &mucJID, const boost::optional<std::string>& password, const boost::optional<std::string>& nickMaybe, bool addAutoJoin, bool createAsReservedIfNew, bool isImpromptu, ChatWindow* reuseChatwindow) {
	MUC::ref muc;
	if (!stanzaChannel_->isAvailable()) {
		/* This is potentially not the optimal solution, but it will avoid consistency issues.*/
		return muc;
	}
	if (addAutoJoin) {
		MUCBookmark bookmark(mucJID, mucJID.getNode());
		bookmark.setAutojoin(true);
		if (nickMaybe) {
			bookmark.setNick(*nickMaybe);
		}
		if (password) {
			bookmark.setPassword(*password);
		}
		mucBookmarkManager_->addBookmark(bookmark);
	}

	std::map<JID, MUCController*>::iterator it = mucControllers_.find(mucJID);
	if (it != mucControllers_.end()) {
		it->second->rejoin();
	} else {
		std::string nick = (nickMaybe && !(*nickMaybe).empty()) ? nickMaybe.get() : nickResolver_->jidToNick(jid_);
		muc = mucManager->createMUC(mucJID);
		if (createAsReservedIfNew) {
			muc->setCreateAsReservedIfNew();
		}
		if (isImpromptu) {
			muc->setCreateAsReservedIfNew();
		}

		MUCController* controller = NULL;
		SingleChatWindowFactoryAdapter* chatWindowFactoryAdapter = NULL;
		if (reuseChatwindow) {
			chatWindowFactoryAdapter = new SingleChatWindowFactoryAdapter(reuseChatwindow);
		}
		boost::shared_ptr<ChatMessageParser> chatMessageParser = boost::make_shared<ChatMessageParser>(emoticons_, highlightManager_->getRules(), true); /* a message parser that knows this is a room/MUC (not a chat) */
		controller = new MUCController(jid_, muc, password, nick, stanzaChannel_, iqRouter_, reuseChatwindow ? chatWindowFactoryAdapter : chatWindowFactory_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory_, eventController_, entityCapsProvider_, roster_, historyController_, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser, isImpromptu, autoAcceptMUCInviteDecider_, vcardManager_, mucBookmarkManager_);
		if (chatWindowFactoryAdapter) {
			/* The adapters are only passed to chat windows, which are deleted in their
			 * controllers' dtor, which are deleted in ChatManager's dtor. The adapters
			 * are also deleted there.*/
			chatWindowFactoryAdapters_[controller] = chatWindowFactoryAdapter;
		}

		mucControllers_[mucJID] = controller;
		controller->setAvailableServerFeatures(serverDiscoInfo_);
		controller->onUserLeft.connect(boost::bind(&ChatsManager::handleUserLeftMUC, this, controller));
		controller->onUserJoined.connect(boost::bind(&ChatsManager::handleChatActivity, this, mucJID.toBare(), "", true));
		controller->onUserNicknameChanged.connect(boost::bind(&ChatsManager::handleUserNicknameChanged, this, controller, _1, _2));
		controller->onActivity.connect(boost::bind(&ChatsManager::handleChatActivity, this, mucJID.toBare(), _1, true));
		controller->onUnreadCountChanged.connect(boost::bind(&ChatsManager::handleUnreadCountChanged, this, controller));
		handleChatActivity(mucJID.toBare(), "", true);
	}

	mucControllers_[mucJID]->showChatWindow();
	return muc;
}

void ChatsManager::handleSearchMUCRequest() {
	mucSearchController_->openSearchWindow();
}

void ChatsManager::handleUserNicknameChanged(MUCController* mucController, const std::string& oldNickname, const std::string& newNickname) {
	JID oldMUCChatJID = mucController->getToJID().withResource(oldNickname);
	JID newMUCChatJID = mucController->getToJID().withResource(newNickname);

	SWIFT_LOG(debug) << "nickname change in " << mucController->getToJID().toString() << " from " << oldNickname << " to " << newNickname << std::endl;

	// get current chat controller
	ChatController *chatController = getChatControllerIfExists(oldMUCChatJID);
	if (chatController) {
		// adjust chat controller
		chatController->setToJID(newMUCChatJID);
		nickResolver_->onNickChanged(newMUCChatJID, oldNickname);
		chatControllers_.erase(oldMUCChatJID);
		chatControllers_[newMUCChatJID] = chatController;

		chatController->onActivity.disconnect(boost::bind(&ChatsManager::handleChatActivity, this, oldMUCChatJID, _1, false));
		chatController->onActivity.connect(boost::bind(&ChatsManager::handleChatActivity, this, newMUCChatJID, _1, false));
		/*for (std::list<ChatListWindow::Chat>::iterator i = recentChats_.begin(); i != recentChats_.end(); i++) {
			if (i->jid ==
		}*/
	}
}

void ChatsManager::handleIncomingMessage(boost::shared_ptr<Message> message) {
	JID jid = message->getFrom();
	boost::shared_ptr<MessageEvent> event(new MessageEvent(message));
	bool isInvite = !!message->getPayload<MUCInvitationPayload>();
	bool isMediatedInvite = (message->getPayload<MUCUserPayload>() && message->getPayload<MUCUserPayload>()->getInvite());
	if (isMediatedInvite) {
		jid = (*message->getPayload<MUCUserPayload>()->getInvite()).from;
	}
	if (!event->isReadable() && !message->getPayload<ChatState>() && !message->getPayload<DeliveryReceipt>() && !message->getPayload<DeliveryReceiptRequest>() && !isInvite && !isMediatedInvite && !message->hasSubject()) {
		return;
	}

	// Try to deliver it to a MUC
	if (message->getType() == Message::Groupchat || message->getType() == Message::Error /*|| (isInvite && message->getType() == Message::Normal)*/) {
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

	// check for impromptu invite to potentially auto-accept
	MUCInvitationPayload::ref invite = message->getPayload<MUCInvitationPayload>();
	if (invite && autoAcceptMUCInviteDecider_->isAutoAcceptedInvite(message->getFrom(), invite)) {
		if (invite->getIsContinuation()) {
			// check for existing chat controller for the from JID
			ChatController* controller = getChatControllerIfExists(jid);
			if (controller) {
				ChatWindow* window = controller->detachChatWindow();
				chatControllers_.erase(jid);
				delete controller;
				handleJoinMUCRequest(invite->getJID(), boost::optional<std::string>(), boost::optional<std::string>(), false, false, true, window);
				return;
			}
		} else {
			handleJoinMUCRequest(invite->getJID(), boost::optional<std::string>(), boost::optional<std::string>(), false, false, true);
			return;
		}
	}

	//if not a mucroom
	if (!event->isReadable() && !isInvite && !isMediatedInvite) {
		/* Only route such messages if a window exists, don't open new windows for them.*/
		ChatController* controller = getChatControllerIfExists(jid);
		if (controller) {
			controller->handleIncomingMessage(event);
		}
	} else {
		getChatControllerOrCreate(jid)->handleIncomingMessage(event);
	}
}

void ChatsManager::handleMUCSelectedAfterSearch(const JID& muc) {
	if (joinMUCWindow_) {
		joinMUCWindow_->setMUC(muc.toString());
	}
}

void ChatsManager::handleMUCBookmarkActivated(const MUCBookmark& mucBookmark) {
	uiEventStream_->send(boost::make_shared<JoinMUCUIEvent>(mucBookmark.getRoom(), mucBookmark.getPassword(), mucBookmark.getNick()));
}

void ChatsManager::handleNewFileTransferController(FileTransferController* ftc) {
	ChatController* chatController = getChatControllerOrCreate(ftc->getOtherParty());
	chatController->handleNewFileTransferController(ftc);
	chatController->activateChatWindow();
}

void ChatsManager::handleWhiteboardSessionRequest(const JID& contact, bool senderIsSelf) {
	ChatController* chatController = getChatControllerOrCreate(contact);
	chatController->handleWhiteboardSessionRequest(senderIsSelf);
	chatController->activateChatWindow();
}

void ChatsManager::handleWhiteboardStateChange(const JID& contact, const ChatWindow::WhiteboardSessionState state) {
	ChatController* chatController = getChatControllerOrCreate(contact);
	chatController->handleWhiteboardStateChange(state);
	chatController->activateChatWindow();
	if (state == ChatWindow::WhiteboardAccepted) {
		boost::filesystem::path path;
		JID bareJID = contact.toBare();
		if (avatarManager_) {
			path = avatarManager_->getAvatarPath(bareJID);
		}
		ChatListWindow::Chat chat(bareJID, nickResolver_->jidToNick(bareJID), "", 0, StatusShow::None, path, false);
 		chatListWindow_->addWhiteboardSession(chat);
	} else {
		chatListWindow_->removeWhiteboardSession(contact.toBare());
	}
}

void ChatsManager::handleRecentActivated(const ChatListWindow::Chat& chat) {
	if (chat.isMUC && !chat.impromptuJIDs.empty()) {
		typedef std::pair<std::string, JID> StringJIDPair;
		std::vector<JID> inviteJIDs;
		foreach(StringJIDPair pair, chat.impromptuJIDs) {
			inviteJIDs.push_back(pair.second);
		}
		uiEventStream_->send(boost::make_shared<CreateImpromptuMUCUIEvent>(inviteJIDs, chat.jid, ""));
	}
	else if (chat.isMUC) {
		/* FIXME: This means that recents requiring passwords will just flat-out not work */
		uiEventStream_->send(boost::make_shared<JoinMUCUIEvent>(chat.jid, boost::optional<std::string>(), chat.nick));
	}
	else {
		uiEventStream_->send(boost::make_shared<RequestChatUIEvent>(chat.jid));
	}
}

void ChatsManager::handleLocalServiceFound(const JID& service, boost::shared_ptr<DiscoInfo> info) {
	foreach (DiscoInfo::Identity identity, info->getIdentities()) {
			if ((identity.getCategory() == "directory"
				&& identity.getType() == "chatroom")
				|| (identity.getCategory() == "conference"
				&& identity.getType() == "text")) {
				localMUCServiceJID_ = service;
				localMUCServiceFinderWalker_->endWalk();
				SWIFT_LOG(debug) << "Use following MUC service for impromptu chats: " << localMUCServiceJID_ << std::endl;
				break;
			}
	}
}

void ChatsManager::handleLocalServiceWalkFinished() {
	onImpromptuMUCServiceDiscovered(!localMUCServiceJID_.toString().empty());
}

std::vector<ChatListWindow::Chat> ChatsManager::getRecentChats() const {
	return std::vector<ChatListWindow::Chat>(recentChats_.begin(), recentChats_.end());
}

std::vector<Contact::ref> Swift::ChatsManager::getContacts(bool withMUCNicks) {
	std::vector<Contact::ref> result;
	foreach (ChatListWindow::Chat chat, recentChats_) {
		if (!chat.isMUC) {
			result.push_back(boost::make_shared<Contact>(chat.chatName.empty() ? chat.jid.toString() : chat.chatName, chat.jid, chat.statusType, chat.avatarPath));
		}
	}
	if (withMUCNicks) {
		/* collect MUC nicks */
		typedef std::map<JID, MUCController*>::value_type Item;
		foreach (const Item& item, mucControllers_) {
			JID mucJID = item.second->getToJID();
			std::map<std::string, JID> participants = item.second->getParticipantJIDs();
			typedef std::map<std::string, JID>::value_type ParticipantType;
			foreach (const ParticipantType& participant, participants) {
				const JID nickJID = JID(mucJID.getNode(), mucJID.getDomain(), participant.first);
				Presence::ref presence = presenceOracle_->getLastPresence(nickJID);
				const boost::filesystem::path avatar = avatarManager_->getAvatarPath(nickJID);
				result.push_back(boost::make_shared<Contact>(participant.first, JID(), presence->getShow(), avatar));
			}
		}
	}
	return result;
}

ChatsManager::SingleChatWindowFactoryAdapter::SingleChatWindowFactoryAdapter(ChatWindow* chatWindow) : chatWindow_(chatWindow) {}
ChatsManager::SingleChatWindowFactoryAdapter::~SingleChatWindowFactoryAdapter() {}
ChatWindow* ChatsManager::SingleChatWindowFactoryAdapter::createChatWindow(const JID &, UIEventStream*) {
	return chatWindow_;
}

}
