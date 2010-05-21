/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MUCController_H
#define SWIFTEN_MUCController_H

#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>
#include <boost/signals/connection.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Network/Timer.h"
#include "Swift/Controllers/Chat/ChatControllerBase.h"
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
	class AvatarManager;
	class UIEventStream;
	class TimerFactory;

	class MUCController : public ChatControllerBase {
		public:
			MUCController(const JID& self, const JID &muc, const String &nick, StanzaChannel* stanzaChannel, PresenceSender* presenceSender, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, PresenceOracle* presenceOracle, AvatarManager* avatarManager, UIEventStream* events, bool useDelayForLatency, TimerFactory* timerFactory);
			~MUCController();
			boost::signal<void ()> onUserLeft;
		
		protected:
			void preSendMessageRequest(boost::shared_ptr<Message> message);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			String senderDisplayNameFromMessage(const JID& from);

		private:
			void handleWindowClosed();
			void handleAvatarChanged(const JID& jid, const String&);
			void handleOccupantJoined(const MUCOccupant& occupant);
			void handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const String& reason);
			void handleOccupantPresenceChange(boost::shared_ptr<Presence> presence);
			void handleJoinComplete(MUC::JoinResult result);
			void handleJoinTimeoutTick();

		private:
			MUC* muc_;
			UIEventStream* events_;
			String nick_;
			Roster* roster_;
			bool parting_;
			bool joined_;
			boost::bsignals::scoped_connection avatarChangedConnection_;
			boost::shared_ptr<Timer> loginCheckTimer_;
	};
}
#endif

