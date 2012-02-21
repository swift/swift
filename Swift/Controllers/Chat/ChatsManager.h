/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>
#include <Swift/Controllers/UIInterfaces/ChatListWindow.h>
#include <Swiften/MUC/MUCBookmark.h>

namespace Swift {
	class EventController;
	class ChatController;
	class ChatControllerBase;
	class MUCController;
	class MUCManager;
	class ChatWindowFactory;
	class JoinMUCWindow;
	class JoinMUCWindowFactory;
	class NickResolver;
	class PresenceOracle;
	class AvatarManager;
	class StanzaChannel;
	class IQRouter;
	class PresenceSender;
	class MUCBookmarkManager;
	class ChatListWindowFactory;
	class TimerFactory;
	class EntityCapsProvider;
	class DirectedPresenceSender;
	class MUCSearchWindowFactory;
	class ProfileSettingsProvider;
	class MUCSearchController;
	class FileTransferOverview;
	class FileTransferController;
	class XMPPRoster;
	class SettingsProvider;
	
	class ChatsManager {
		public:
			ChatsManager(JID jid, StanzaChannel* stanzaChannel, IQRouter* iqRouter, EventController* eventController, ChatWindowFactory* chatWindowFactory, JoinMUCWindowFactory* joinMUCWindowFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, PresenceSender* presenceSender, UIEventStream* uiEventStream, ChatListWindowFactory* chatListWindowFactory, bool useDelayForLatency, TimerFactory* timerFactory, MUCRegistry* mucRegistry, EntityCapsProvider* entityCapsProvider, MUCManager* mucManager, MUCSearchWindowFactory* mucSearchWindowFactory, ProfileSettingsProvider* profileSettings, FileTransferOverview* ftOverview, XMPPRoster* roster, bool eagleMode, SettingsProvider* settings);
			virtual ~ChatsManager();
			void setAvatarManager(AvatarManager* avatarManager);
			void setOnline(bool enabled);
			void setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<Message> message);

		private:
			ChatListWindow::Chat createChatListChatItem(const JID& jid, const std::string& activity);
			void handleChatRequest(const std::string& contact);
			void handleJoinMUCRequest(const JID& muc, const boost::optional<std::string>& password, const boost::optional<std::string>& nick, bool addAutoJoin, bool createAsReservedIfNew);
			void handleSearchMUCRequest();
			void handleMUCSelectedAfterSearch(const JID&);
			void rebindControllerJID(const JID& from, const JID& to);
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleMUCBookmarkAdded(const MUCBookmark& bookmark);
			void handleMUCBookmarkRemoved(const MUCBookmark& bookmark);
			void handleUserLeftMUC(MUCController* mucController);
			void handleBookmarksReady();
			void handleChatActivity(const JID& jid, const std::string& activity, bool isMUC);
			void handleNewFileTransferController(FileTransferController*);
			void appendRecent(const ChatListWindow::Chat& chat);
			void prependRecent(const ChatListWindow::Chat& chat);
			void setupBookmarks();
			void loadRecents();
			void saveRecents();
			void handleChatMadeRecent();
			void handleMUCBookmarkActivated(const MUCBookmark&);
			void handleRecentActivated(const ChatListWindow::Chat&);
			void handleUnreadCountChanged(ChatControllerBase* controller);
			void handleAvatarChanged(const JID& jid);
			void handleClearRecentsRequested();
			void handleJIDAddedToRoster(const JID&);
			void handleJIDRemovedFromRoster(const JID&);
			void handleJIDUpdatedInRoster(const JID&);
			void handleRosterCleared();
			void handleSettingChanged(const std::string& settingPath);

			void updatePresenceReceivingStateOnChatController(const JID&);

			ChatController* getChatControllerOrFindAnother(const JID &contact);
			ChatController* createNewChatController(const JID &contact);
			ChatController* getChatControllerOrCreate(const JID &contact);
			ChatController* getChatControllerIfExists(const JID &contact, bool rebindIfNeeded = true);

		private:
			std::map<JID, MUCController*> mucControllers_;
			std::map<JID, ChatController*> chatControllers_;
			EventController* eventController_;
			JID jid_;
			StanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;
			ChatWindowFactory* chatWindowFactory_;
			JoinMUCWindowFactory* joinMUCWindowFactory_;
			NickResolver* nickResolver_;
			PresenceOracle* presenceOracle_;
			AvatarManager* avatarManager_;
			PresenceSender* presenceSender_;
			UIEventStream* uiEventStream_;
			MUCBookmarkManager* mucBookmarkManager_;
			boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
			ChatListWindow* chatListWindow_;
			JoinMUCWindow* joinMUCWindow_;
			boost::bsignals::scoped_connection uiEventConnection_;
			bool useDelayForLatency_;
			TimerFactory* timerFactory_;
			MUCRegistry* mucRegistry_;
			EntityCapsProvider* entityCapsProvider_;
			MUCManager* mucManager;
			MUCSearchController* mucSearchController_;
			std::list<ChatListWindow::Chat> recentChats_;
			ProfileSettingsProvider* profileSettings_;
			FileTransferOverview* ftOverview_;
			XMPPRoster* roster_;
			bool eagleMode_;
			bool userWantsReceipts_;
			SettingsProvider* settings_;
	};
}
