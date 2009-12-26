#pragma once

#include <map>

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/Presence.h"
#include "Swiften/JID/JID.h"
#include "Swiften/MUC/MUCRegistry.h"

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
	class ChatsManager : public MUCRegistry {
		public:
			ChatsManager(JID jid, StanzaChannel* stanzaChannel, IQRouter* iqRouter, EventController* eventController, ChatWindowFactory* chatWindowFactory, TreeWidgetFactory* treeWidgetFactory, NickResolver* nickResolver, PresenceOracle* presenceOracle, boost::shared_ptr<DiscoInfo> serverDiscoInfo, PresenceSender* presenceSender);
			~ChatsManager();
			void setAvatarManager(AvatarManager* avatarManager);
			void setEnabled(bool enabled);
			void setServerDiscoInfo(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<Message> message);
			void handleChatRequest(const String& contact);
			void handleJoinMUCRequest(const JID& muc, const String& nick);
		private:
			void rebindControllerJID(const JID& from, const JID& to);
			void handlePresenceChange(boost::shared_ptr<Presence> oldPresence, boost::shared_ptr<Presence> newPresence);
			ChatController* getChatController(const JID &contact);
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
			boost::shared_ptr<DiscoInfo> serverDiscoInfo_;
	};
}
