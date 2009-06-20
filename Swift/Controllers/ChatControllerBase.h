#ifndef SWIFTEN_ChatControllerBase_H
#define SWIFTEN_ChatControllerBase_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Events/MessageEvent.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"
#include "Swiften/Elements/Error.h"
#include "Swiften/Presence/PresenceOracle.h"

namespace Swift {
	class IQRouter;
	class StanzaChannel;
	class ChatWindow;
	class ChatWindowFactory;
	class AvatarManager;

	class ChatControllerBase  {
		public:
			virtual ~ChatControllerBase();
			void showChatWindow();
			void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<MessageEvent> message);

		protected:
			ChatControllerBase(StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager);

			virtual void postSendMessage(const String&) {};
			virtual String senderDisplayNameFromMessage(JID from);
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> previousPresence);
			virtual bool isIncomingMessageFromMe(boost::shared_ptr<Message>) = 0;
			virtual void preHandleIncomingMessage(boost::shared_ptr<Message>) {};
			virtual void preSendMessageRequest(boost::shared_ptr<Message>) {};

		private:
			void handleSendMessageRequest(const String &body);
			String getStatusChangeString(boost::shared_ptr<Presence> presence);
			void handleAllMessagesRead();
			void handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog>, const boost::optional<Error>& error);
			String getErrorMessage(boost::shared_ptr<Error>);

		protected:
			std::vector<boost::shared_ptr<MessageEvent> > unreadMessages_;
			StanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;
			ChatWindowFactory* chatWindowFactory_;
			ChatWindow* chatWindow_;
			JID toJID_;
			bool labelsEnabled_;
			PresenceOracle* presenceOracle_;
			AvatarManager* avatarManager_;
	};
}

#endif
