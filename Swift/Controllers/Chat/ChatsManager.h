/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swiften/MUC/MUCBookmark.h"

namespace Swift {
	class EventController;
	class ChatController;
	class MUCController;
	class ChatWindowFactory;
	class TreeWidgetFactory;
	class NickResolver;
	class PresenceOracle;
	class AvatarManager;
	class StanzaChannel;
	class IQRouter;
	class PresenceSender;
	class MUCBookmarkManager;
	class ChatListWindow;
	class ChatListWindowFactory;

	class ChatsManager : public MUCRegistry {
		public:
			ChatsManager(JID jid, StanzaChannel* stanzaChannel, IQRouter* iqRouter, EventController* eventController, ChatWindowFactory* chatWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, boost::shared_ptr<DiscoInfo> serverDiscoInfo, PresenceSender* presenceSender, UIEventStream* uiEventStream, ChatListWindowFactory* chatListWindowFactory, bool useDelayForLatency);
			~ChatsManager();
			void setAvatarManager(AvatarManager* avatarManager);
			void setEnabled(bool enabled);
			void setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<Message> message);
			void handleChatRequest(const String& contact);
		private:
			void handleJoinMUCRequest(const JID& muc, const boost::optional<String>& nick);
			void rebindControllerJID(const JID& from, const JID& to);
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> lastPresence);
			void handleUIEvent(boost::shared_ptr<UIEvent> event);
			void handleMUCBookmarkAdded(boost::shared_ptr<MUCBookmark> bookmark);
			void handleMUCBookmarkRemoved(boost::shared_ptr<MUCBookmark> bookmark);
			void handleUserLeftMUC(MUCController* mucController);
			ChatController* getChatControllerOrFindAnother(const JID &contact);
			ChatController* createNewChatController(const JID &contact);
			ChatController* getChatControllerOrCreate(const JID &contact);
			ChatController* getChatControllerIfExists(const JID &contact);
			virtual bool isMUC(const JID& muc) const;

			std::map<JID, MUCController*> mucControllers_;
			std::map<JID, ChatController*> chatControllers_;
			EventController* eventController_;
			JID jid_;
			StanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;;
			ChatWindowFactory* chatWindowFactory_;
			TreeWidgetFactory* treeWidgetFactory_;
			NickResolver* nickResolver_;
			PresenceOracle* presenceOracle_;
			AvatarManager* avatarManager_;
			PresenceSender* presenceSender_;
			UIEventStream* uiEventStream_;
			MUCBookmarkManager* mucBookmarkManager_;
			boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
			ChatListWindow* chatListWindow_;
			boost::bsignals::scoped_connection uiEventConnection_;
			bool useDelayForLatency_;
	};
}
