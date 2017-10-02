/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Chat/ChatsManager.h>

#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <Swiften/Avatars/AvatarManager.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Disco/DiscoServiceWalker.h>
#include <Swiften/Disco/FeatureOracle.h>
#include <Swiften/Elements/CarbonsReceived.h>
#include <Swiften/Elements/CarbonsSent.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Elements/Forwarded.h>
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
#include <Swift/Controllers/UIEvents/SendFileUIEvent.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindowFactory.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>
#include <Swift/Controllers/UIInterfaces/JoinMUCWindowFactory.h>
#include <Swift/Controllers/WhiteboardManager.h>
#include <Swift/Controllers/XMPPEvents/EventController.h>

BOOST_CLASS_VERSION(Swift::ChatListWindow::Chat, 2)

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
        if (version > 1) {
            ar & chat.inviteesNames;
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
            vcardManager_(vcardManager) {
    timerFactory_ = timerFactory;
    eventController_ = eventController;
    stanzaChannel_ = stanzaChannel;
    iqRouter_ = iqRouter;
    chatWindowFactory_ = chatWindowFactory;
    nickResolver_ = nickResolver;
    presenceOracle_ = presenceOracle;
    avatarManager_ = nullptr;
    serverDiscoInfo_ = std::make_shared<DiscoInfo>();
    presenceSender_ = presenceSender;
    uiEventStream_ = uiEventStream;
    mucBookmarkManager_ = nullptr;
    profileSettings_ = profileSettings;
    presenceOracle_->onPresenceChange.connect(boost::bind(&ChatsManager::handlePresenceChange, this, _1));
    uiEventConnection_ = uiEventStream_->onUIEvent.connect(boost::bind(&ChatsManager::handleUIEvent, this, _1));

    chatListWindow_ = chatListWindowFactory->createChatListWindow(uiEventStream_);
    chatListWindow_->onMUCBookmarkActivated.connect(boost::bind(&ChatsManager::handleMUCBookmarkActivated, this, _1));
    chatListWindow_->onRecentActivated.connect(boost::bind(&ChatsManager::handleRecentActivated, this, _1));
    chatListWindow_->onClearRecentsRequested.connect(boost::bind(&ChatsManager::handleClearRecentsRequested, this));

    joinMUCWindow_ = nullptr;
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
    ftOverview_->onNewFileTransferController.disconnect(boost::bind(&ChatsManager::handleNewFileTransferController, this, _1));
    delete joinMUCWindow_;
    for (JIDChatControllerPair controllerPair : chatControllers_) {
        delete controllerPair.second;
    }
    for (JIDMUCControllerPair controllerPair : mucControllers_) {
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
        for (ChatListWindow::Chat& chat : recentsLimited) {
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

    oa & recentsLimited;
    std::string serializedStr = Base64::encode(createByteArray(serializeStream.str()));
    profileSettings_->storeString(RECENT_CHATS, serializedStr);
}

void ChatsManager::handleClearRecentsRequested() {
    recentChats_.clear();
    saveRecents();
    handleUnreadCountChanged(nullptr);
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
    /*    Setting that all chat controllers aren't receiving presence anymore;
        including MUC 1-to-1 chats due to the assumtion that this handler
        is only called on log out. */
    for (JIDChatControllerPair pair : chatControllers_) {
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
        Presence::ref presence = presenceOracle_->getAccountPresence(fixedChat.jid.toBare());
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
        for (std::string recentString : recents) {
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

        for (auto chat : recentChats) {
            chat.statusType = StatusShow::None;
            chat = updateChatStatusAndAvatarHelper(chat);
            prependRecent(chat);
        }
    }
    handleUnreadCountChanged(nullptr);
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

                std::map<JID, std::string> participantsNames;
                for (auto& i : invitees_[jid]) {
                    participantsNames.emplace(i, roster_->getNameForJID(i));
                }
                chat.inviteesNames = participantsNames;
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
        Presence::ref presence = presenceOracle_->getAccountPresence(bareishJID);
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
    handleUnreadCountChanged(nullptr);
    saveRecents();

    // Look up potential MUC controller and update title accordingly as people
    // join impromptu chats.
    if (mucControllers_.find(jid) != mucControllers_.end()) {
        auto chatListWindowIter = std::find_if(recentChats_.begin(), recentChats_.end(), [&](const ChatListWindow::Chat& chatListWindow) { return jid == (chatListWindow.jid); });
        if (chatListWindowIter != recentChats_.end() && (mucControllers_[jid]->isImpromptu() || !chatListWindowIter->impromptuJIDs.empty())) {
            mucControllers_[jid]->setChatWindowTitle(chatListWindowIter->getTitle());
        }
    }
}

void ChatsManager::handleChatClosed(const JID& /*jid*/) {
    cleanupPrivateMessageRecents();
    chatListWindow_->setRecents(recentChats_);
}

void ChatsManager::handleUnreadCountChanged(ChatControllerBase* controller) {
    int unreadTotal = 0;
    bool controllerIsMUC = dynamic_cast<MUCController*>(controller);
    bool isPM = controller && !controllerIsMUC && mucRegistry_->isMUC(controller->getToJID().toBare());
    for (ChatListWindow::Chat& chatItem : recentChats_) {
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
    for (const ChatListWindow::Chat& chat : chats) {
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
    if (oldChat) {
        mergedChat.inviteesNames.insert(oldChat->inviteesNames.begin(), oldChat->inviteesNames.end());
        mergedChat.impromptuJIDs.insert(oldChat->impromptuJIDs.begin(), oldChat->impromptuJIDs.end());
    }
    recentChats_.push_front(mergedChat);
}

void ChatsManager::prependRecent(const ChatListWindow::Chat& chat) {
    boost::optional<ChatListWindow::Chat> oldChat = removeExistingChat(chat);
    ChatListWindow::Chat mergedChat = chat;
    if (oldChat) {
        mergedChat.inviteesNames.insert(oldChat->inviteesNames.begin(), oldChat->inviteesNames.end());
        mergedChat.impromptuJIDs.insert(oldChat->impromptuJIDs.begin(), oldChat->impromptuJIDs.end());
    }
    recentChats_.push_back(mergedChat);
}

void ChatsManager::handleUserLeftMUC(MUCController* mucController) {
    std::map<JID, MUCController*>::iterator it;
    for (it = mucControllers_.begin(); it != mucControllers_.end(); ++it) {
        if ((*it).second == mucController) {
            for (ChatListWindow::Chat& chat : recentChats_) {
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
    for (StringMUCOccupantPair occupant : occupants) {
        boost::optional<JID> realJID = occupant.second.getRealJID();
        if (realJID) {
            missingJIDsToInvite.erase(std::remove(missingJIDsToInvite.begin(), missingJIDsToInvite.end(), realJID->toBare()), missingJIDsToInvite.end());
        }
    }

    if (reuseChatJID) {
        muc->invitePerson(reuseChatJID.get(), reason, true, true);
    }
    for (const JID& jid : missingJIDsToInvite) {
        muc->invitePerson(jid, reason, true);
    }
}

void ChatsManager::handleUIEvent(std::shared_ptr<UIEvent> event) {
    std::shared_ptr<RequestChatUIEvent> chatEvent = std::dynamic_pointer_cast<RequestChatUIEvent>(event);
    if (chatEvent) {
        handleChatRequest(chatEvent->getContact());
        return;
    }
    std::shared_ptr<RemoveMUCBookmarkUIEvent> removeMUCBookmarkEvent = std::dynamic_pointer_cast<RemoveMUCBookmarkUIEvent>(event);
    if (removeMUCBookmarkEvent) {
        mucBookmarkManager_->removeBookmark(removeMUCBookmarkEvent->getBookmark());
        return;
    }
    std::shared_ptr<AddMUCBookmarkUIEvent> addMUCBookmarkEvent = std::dynamic_pointer_cast<AddMUCBookmarkUIEvent>(event);
    if (addMUCBookmarkEvent) {
        mucBookmarkManager_->addBookmark(addMUCBookmarkEvent->getBookmark());
        return;
    }
    std::shared_ptr<SendFileUIEvent> sendFileEvent = std::dynamic_pointer_cast<SendFileUIEvent>(event);
    if (sendFileEvent) {
        JID fileReceiver = sendFileEvent->getJID();
        if (fileReceiver.isBare()) {
            // See if there is a chat controller for a conversation with a bound
            // full JID. Check if this JID supports file transfer and use it instead
            // of the bare JID.
            ChatController* controller = getChatControllerIfExists(fileReceiver, false);
            if (controller) {
                JID controllerJID = controller->getToJID();
                if (!controllerJID.isBare() && (FeatureOracle(entityCapsProvider_, presenceOracle_).isFileTransferSupported(controllerJID) == Yes)) {
                    fileReceiver = controllerJID;
                }
            }
        }
        ftOverview_->sendFile(fileReceiver, sendFileEvent->getFilename());
        return;
    }

    std::shared_ptr<CreateImpromptuMUCUIEvent> createImpromptuMUCEvent = std::dynamic_pointer_cast<CreateImpromptuMUCUIEvent>(event);
    if (createImpromptuMUCEvent) {
        assert(!localMUCServiceJID_.toString().empty());
        // The room JID is random for new impromptu rooms, or a predefined JID for impromptu rooms resumed from the 'Recent chats' list.
        JID roomJID = createImpromptuMUCEvent->getRoomJID().toString().empty() ? JID(idGenerator_.generateID(), localMUCServiceJID_) : createImpromptuMUCEvent->getRoomJID();

        std::vector<JID> missingJIDsToInvite = createImpromptuMUCEvent->getJIDs();
        for (const JID& jid : missingJIDsToInvite) {
            invitees_[roomJID].insert(jid);
        }
        // join muc
        MUC::ref muc = handleJoinMUCRequest(roomJID, boost::optional<std::string>(), nickResolver_->jidToNick(jid_), false, true, true);
        mucControllers_[roomJID]->onImpromptuConfigCompleted.connect(boost::bind(&ChatsManager::finalizeImpromptuJoin, this, muc, createImpromptuMUCEvent->getJIDs(), createImpromptuMUCEvent->getReason(), boost::optional<JID>()));
        mucControllers_[roomJID]->activateChatWindow();
    }

    std::shared_ptr<EditMUCBookmarkUIEvent> editMUCBookmarkEvent = std::dynamic_pointer_cast<EditMUCBookmarkUIEvent>(event);
    if (editMUCBookmarkEvent) {
        mucBookmarkManager_->replaceBookmark(editMUCBookmarkEvent->getOldBookmark(), editMUCBookmarkEvent->getNewBookmark());
    }
    else if (JoinMUCUIEvent::ref joinEvent = std::dynamic_pointer_cast<JoinMUCUIEvent>(event)) {
        handleJoinMUCRequest(joinEvent->getJID(), joinEvent->getPassword(), joinEvent->getNick(), joinEvent->getShouldJoinAutomatically(), joinEvent->getCreateAsReservedRoomIfNew(), joinEvent->isImpromptu());
        mucControllers_[joinEvent->getJID()]->activateChatWindow();
    }
    else if (std::shared_ptr<RequestJoinMUCUIEvent> joinEvent = std::dynamic_pointer_cast<RequestJoinMUCUIEvent>(event)) {
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
    for (ChatListWindow::Chat& chat : recentChats_) {
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
void ChatsManager::handlePresenceChange(std::shared_ptr<Presence> newPresence) {
    if (mucRegistry_->isMUC(newPresence->getFrom().toBare())) return;

    for (ChatListWindow::Chat& chat : recentChats_) {
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
    for (ChatListWindow::Chat& chat : recentChats_) {
        if (!chat.isMUC) {
            chat.setAvatarPath(avatarManager_->getAvatarPath(chat.jid));
        }
    }
    avatarManager_->onAvatarChanged.connect(boost::bind(&ChatsManager::handleAvatarChanged, this, _1));
}

void ChatsManager::handleAvatarChanged(const JID& jid) {
    for (ChatListWindow::Chat& chat : recentChats_) {
        if (!chat.isMUC && jid.toBare() == chat.jid.toBare()) {
            chat.setAvatarPath(avatarManager_->getAvatarPath(jid));
            break;
        }
    }
}

void ChatsManager::setServerDiscoInfo(std::shared_ptr<DiscoInfo> info) {
    serverDiscoInfo_ = info;
    for (JIDChatControllerPair pair : chatControllers_) {
        pair.second->setAvailableServerFeatures(info);
    }
    for (JIDMUCControllerPair pair : mucControllers_) {
        pair.second->setAvailableServerFeatures(info);
    }
}

/**
 * This is to be called on connect/disconnect.
 */
void ChatsManager::setOnline(bool enabled) {
    for (JIDChatControllerPair controllerPair : chatControllers_) {
        controllerPair.second->setOnline(enabled);
    }
    for (JIDMUCControllerPair controllerPair : mucControllers_) {
        controllerPair.second->setOnline(enabled);
        if (enabled) {
            controllerPair.second->rejoin();
        }
    }
    if (!enabled) {
        markAllRecentsOffline();
    } else {
        setupBookmarks();
        localMUCServiceJID_ = JID();
        localMUCServiceFinderWalker_ = std::make_shared<DiscoServiceWalker>(jid_.getDomain(), iqRouter_);
        localMUCServiceFinderWalker_->onServiceFound.connect(boost::bind(&ChatsManager::handleLocalServiceFound, this, _1, _2));
        localMUCServiceFinderWalker_->onWalkAborted.connect(boost::bind(&ChatsManager::handleLocalServiceWalkFinished, this));
        localMUCServiceFinderWalker_->onWalkComplete.connect(boost::bind(&ChatsManager::handleLocalServiceWalkFinished, this));
        localMUCServiceFinderWalker_->beginWalk();
    }

    if (chatListWindow_) {
        chatListWindow_->setBookmarksEnabled(enabled);
    }
}

void ChatsManager::handleChatRequest(const std::string &contact) {
    ChatController* controller = getChatControllerOrFindAnother(JID(contact));
    controller->activateChatWindow();
}

ChatController* ChatsManager::getChatControllerOrFindAnother(const JID &contact) {
    ChatController* controller = getChatControllerIfExists(contact);
    if (!controller && !mucRegistry_->isMUC(contact.toBare())) {
        for (JIDChatControllerPair pair : chatControllers_) {
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
    std::shared_ptr<ChatMessageParser> chatMessageParser = std::make_shared<ChatMessageParser>(emoticons_, highlightManager_->getConfiguration(), ChatMessageParser::Mode::Chat); /* a message parser that knows this is a chat (not a room/MUC) */
    ChatController* controller = new ChatController(jid_, stanzaChannel_, iqRouter_, chatWindowFactory_, contact, nickResolver_, presenceOracle_, avatarManager_, mucRegistry_->isMUC(contact.toBare()), useDelayForLatency_, uiEventStream_, timerFactory_, eventController_, entityCapsProvider_, userWantsReceipts_, settings_, historyController_, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser, autoAcceptMUCInviteDecider_);
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
            return nullptr;
        }
        //Need to look for an unbound window to bind first
        JID bare(contact.toBare());
        if (chatControllers_.find(bare) != chatControllers_.end()) {
            if (rebindIfNeeded) {
                rebindControllerJID(bare, contact);
            }
            else {
                return chatControllers_[bare];
            }
        } else {
            for (JIDChatControllerPair pair : chatControllers_) {
                if (pair.first.toBare() == contact.toBare()) {
                    if (rebindIfNeeded) {
                        rebindControllerJID(pair.first, contact);
                        return chatControllers_[contact];
                    } else {
                        return pair.second;
                    }
                }
            }
            return nullptr;
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
        if (stanzaChannel_->isAvailable()) {
            it->second->rejoin();
        }
    } else {
        std::string nick = (nickMaybe && !(*nickMaybe).empty()) ? nickMaybe.get() : nickResolver_->jidToNick(jid_);
        muc = mucManager->createMUC(mucJID);
        if (createAsReservedIfNew) {
            muc->setCreateAsReservedIfNew();
        }
        if (isImpromptu) {
            muc->setCreateAsReservedIfNew();
        }

        MUCController* controller = nullptr;
        SingleChatWindowFactoryAdapter* chatWindowFactoryAdapter = nullptr;
        if (reuseChatwindow) {
            chatWindowFactoryAdapter = new SingleChatWindowFactoryAdapter(reuseChatwindow);
        }
        std::shared_ptr<ChatMessageParser> chatMessageParser = std::make_shared<ChatMessageParser>(emoticons_, highlightManager_->getConfiguration(), ChatMessageParser::Mode::GroupChat); /* a message parser that knows this is a room/MUC (not a chat) */
        controller = new MUCController(jid_, muc, password, nick, stanzaChannel_, iqRouter_, reuseChatwindow ? chatWindowFactoryAdapter : chatWindowFactory_, nickResolver_, presenceOracle_, avatarManager_, uiEventStream_, false, timerFactory_, eventController_, entityCapsProvider_, roster_, historyController_, mucRegistry_, highlightManager_, clientBlockListManager_, chatMessageParser, isImpromptu, autoAcceptMUCInviteDecider_, vcardManager_, mucBookmarkManager_);
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
        if (!stanzaChannel_->isAvailable()) {
            /* When online, the MUC is added to the registry in MUCImpl::internalJoin. This method is not
             * called when Swift is offline, so we add it here as only MUCs in the registry are rejoined
             * when going back online.
             */
            mucRegistry_->addMUC(mucJID.toBare());
        }
        handleChatActivity(mucJID.toBare(), "", true);
    }

    auto chatListWindowIter = std::find_if(recentChats_.begin(), recentChats_.end(), [&](const ChatListWindow::Chat& chatListWindow) { return mucJID == (chatListWindow.jid); });
    if (chatListWindowIter != recentChats_.end() && (mucControllers_[mucJID]->isImpromptu() || !chatListWindowIter->impromptuJIDs.empty())) {
        mucControllers_[mucJID]->setChatWindowTitle(chatListWindowIter->getTitle());
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
    }
}

bool ChatsManager::messageCausesSessionBinding(std::shared_ptr<Message> message) {
    bool causesRebind = false;
    ChatState::ref chatState = message->getPayload<ChatState>();
    if (!message->getBody().get_value_or("").empty() || (chatState && chatState->getChatState() == ChatState::Composing)) {
        causesRebind = true;
    }
    return causesRebind;
}

void ChatsManager::handleIncomingMessage(std::shared_ptr<Message> incomingMessage) {
    std::shared_ptr<Message> message = incomingMessage;
    if (message->getFrom().toBare() == jid_.toBare()) {
        CarbonsReceived::ref carbonsReceived;
        CarbonsSent::ref carbonsSent;
        Forwarded::ref forwarded;
        Message::ref forwardedMessage;
        if ((carbonsReceived = incomingMessage->getPayload<CarbonsReceived>()) &&
            (forwarded = carbonsReceived->getForwarded()) &&
            (forwardedMessage = std::dynamic_pointer_cast<Message>(forwarded->getStanza()))) {
            message = forwardedMessage;
        }
        else if ((carbonsSent = incomingMessage->getPayload<CarbonsSent>()) &&
                 (forwarded = carbonsSent->getForwarded()) &&
                 (forwardedMessage = std::dynamic_pointer_cast<Message>(forwarded->getStanza()))) {
            JID toJID = forwardedMessage->getTo();

            ChatController* controller = getChatControllerOrCreate(toJID);
            if (controller) {
                controller->handleIncomingOwnMessage(forwardedMessage);
            }
            else {
                SWIFT_LOG(error) << "Carbons message ignored." << std::endl;
            }
            return;
        }
    }
    JID fromJID = message->getFrom();

    std::shared_ptr<MessageEvent> event(new MessageEvent(message));
    bool isInvite = !!message->getPayload<MUCInvitationPayload>();
    bool isMediatedInvite = (message->getPayload<MUCUserPayload>() && message->getPayload<MUCUserPayload>()->getInvite());
    if (isMediatedInvite) {
        fromJID = (*message->getPayload<MUCUserPayload>()->getInvite()).from;
    }
    if (!event->isReadable() && !message->getPayload<ChatState>() && !message->getPayload<DeliveryReceipt>() && !message->getPayload<DeliveryReceiptRequest>() && !isInvite && !isMediatedInvite && !message->hasSubject()) {
        return;
    }

    // Try to deliver MUC errors to a MUC PM window if a suitable window is open.
    if (message->getType() == Message::Error) {
        auto controller = getChatControllerIfExists(fromJID, messageCausesSessionBinding(message));
        if (controller) {
            controller->handleIncomingMessage(event);
            return;
        }
    }

    // Try to deliver it to a MUC.
    if (message->getType() == Message::Groupchat || message->getType() == Message::Error) {
        // Try to deliver it to a MUC room.
        std::map<JID, MUCController*>::iterator i = mucControllers_.find(fromJID.toBare());
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
            ChatController* controller = getChatControllerIfExists(fromJID);
            if (controller) {
                ChatWindow* window = controller->detachChatWindow();
                chatControllers_.erase(fromJID);
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

        // Do not bind a controller to a full JID, for delivery receipts or chat state notifications.
        ChatController* controller = getChatControllerIfExists(fromJID, messageCausesSessionBinding(message));
        if (controller) {
            controller->handleIncomingMessage(event);
        }
    } else {
        getChatControllerOrCreate(fromJID)->handleIncomingMessage(event);
    }
}

void ChatsManager::handleMUCSelectedAfterSearch(const JID& muc) {
    if (joinMUCWindow_) {
        joinMUCWindow_->setMUC(muc.toString());
    }
}

void ChatsManager::handleMUCBookmarkActivated(const MUCBookmark& mucBookmark) {
    uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(mucBookmark.getRoom(), mucBookmark.getPassword(), mucBookmark.getNick()));
}

void ChatsManager::handleNewFileTransferController(FileTransferController* ftc) {
    ChatController* chatController = getChatControllerOrCreate(ftc->getOtherParty());
    chatController->handleNewFileTransferController(ftc);
    if (!ftc->isIncoming()) {
        chatController->activateChatWindow();
    }
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
        for (StringJIDPair pair : chat.impromptuJIDs) {
            inviteJIDs.push_back(pair.second);
        }
        uiEventStream_->send(std::make_shared<CreateImpromptuMUCUIEvent>(inviteJIDs, chat.jid, ""));
    }
    else if (chat.isMUC) {
        bool isImpromptu = (!chat.inviteesNames.empty() || !chat.impromptuJIDs.empty());
        /* FIXME: This means that recents requiring passwords will just flat-out not work */
        uiEventStream_->send(std::make_shared<JoinMUCUIEvent>(chat.jid, boost::optional<std::string>(), chat.nick, false, false, isImpromptu));
    }
    else {
        uiEventStream_->send(std::make_shared<RequestChatUIEvent>(chat.jid));
    }
}

void ChatsManager::handleLocalServiceFound(const JID& service, std::shared_ptr<DiscoInfo> info) {
    for (DiscoInfo::Identity identity : info->getIdentities()) {
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
    bool impromptuMUCSupported = !localMUCServiceJID_.toString().empty();
    for (JIDChatControllerPair controllerPair : chatControllers_) {
        controllerPair.second->setCanStartImpromptuChats(impromptuMUCSupported);
    }
    for (JIDMUCControllerPair controllerPair : mucControllers_) {
        controllerPair.second->setCanStartImpromptuChats(impromptuMUCSupported);
    }
    onImpromptuMUCServiceDiscovered(impromptuMUCSupported);
}

std::vector<ChatListWindow::Chat> ChatsManager::getRecentChats() const {
    return std::vector<ChatListWindow::Chat>(recentChats_.begin(), recentChats_.end());
}

std::vector<Contact::ref> Swift::ChatsManager::getContacts(bool withMUCNicks) {
    std::vector<Contact::ref> result;
    for (ChatListWindow::Chat chat : recentChats_) {
        if (!chat.isMUC) {
            result.push_back(std::make_shared<Contact>(chat.chatName.empty() ? chat.jid.toString() : chat.chatName, chat.jid, chat.statusType, chat.avatarPath));
        }
    }
    if (withMUCNicks) {
        /* collect MUC nicks */
        typedef std::map<JID, MUCController*>::value_type Item;
        for (const Item& item : mucControllers_) {
            JID mucJID = item.second->getToJID();
            std::map<std::string, JID> participants = item.second->getParticipantJIDs();
            typedef std::map<std::string, JID>::value_type ParticipantType;
            for (const ParticipantType& participant : participants) {
                const JID nickJID = JID(mucJID.getNode(), mucJID.getDomain(), participant.first);
                Presence::ref presence = presenceOracle_->getLastPresence(nickJID);
                const boost::filesystem::path avatar = avatarManager_->getAvatarPath(nickJID);
                result.push_back(std::make_shared<Contact>(participant.first, JID(), presence->getShow(), avatar));
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
