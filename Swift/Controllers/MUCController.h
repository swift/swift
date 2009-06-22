#ifndef SWIFTEN_MUCController_H
#define SWIFTEN_MUCController_H

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swift/Controllers/ChatControllerBase.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/JID/JID.h"
#include "Swiften/MUC/MUC.h"
#include "Swiften/MUC/MUCOccupant.h"

namespace Swift {
	class StanzaChannel;
	class IQRouter;
	class ChatWindow;
	class ChatWindowFactory;
	class Roster;
	class TreeWidgetFactory;
	class AvatarManager;

	class MUCController : public ChatControllerBase {
		public:
			MUCController(const JID &muc, const String &nick, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, TreeWidgetFactory *treeWidgetFactory, PresenceOracle* presenceOracle, AvatarManager* avatarManager);
			~MUCController();
		
		protected:
			void preSendMessageRequest(boost::shared_ptr<Message> message);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			String senderDisplayNameFromMessage(const JID& from);

		private:
			void handleWindowClosed();
			void handleOccupantJoined(const MUCOccupant& occupant);
			void handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const String& reason);
			void handleOccupantPresenceChange(boost::shared_ptr<Presence> presence);

		private:
			MUC *muc_;
			String nick_;
			TreeWidgetFactory *treeWidgetFactory_;
			Roster *roster_;
	};
}
#endif

