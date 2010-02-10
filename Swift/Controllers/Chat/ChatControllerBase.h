#ifndef SWIFTEN_ChatControllerBase_H
#define SWIFTEN_ChatControllerBase_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/filesystem.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/Events/MessageEvent.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Queries/IQRouter.h"

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
			void activateChatWindow();
			void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<MessageEvent> message);
			void addMessage(const String& message, const String& senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath);
			void setEnabled(bool enabled);
			void setToJID(const JID& jid) {toJID_ = jid;};
		protected:
			ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager);

			virtual void postSendMessage(const String&) {};
			virtual String senderDisplayNameFromMessage(const JID& from) = 0;
			virtual bool isIncomingMessageFromMe(boost::shared_ptr<Message>) = 0;
			virtual void preHandleIncomingMessage(boost::shared_ptr<Message>) {};
			virtual void preSendMessageRequest(boost::shared_ptr<Message>) {};


		private:
			void handleSendMessageRequest(const String &body);
			void handleAllMessagesRead();
			void handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog>, const boost::optional<ErrorPayload>& error);
			String getErrorMessage(boost::shared_ptr<ErrorPayload>);

		protected:
			JID selfJID_;
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
