/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/signals/connection.hpp>
#include <set>

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
	class TabComplete;

	enum JoinPart {Join, Part, JoinThenPart, PartThenJoin};

	struct NickJoinPart {
			NickJoinPart(const String& nick, JoinPart type) : nick(nick), type(type) {};
			String nick;
			JoinPart type;
	};

	class MUCController : public ChatControllerBase {
		public:
			MUCController(const JID& self, const JID &muc, const String &nick, StanzaChannel* stanzaChannel, PresenceSender* presenceSender, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, PresenceOracle* presenceOracle, AvatarManager* avatarManager, UIEventStream* events, bool useDelayForLatency, TimerFactory* timerFactory, EventController* eventController);
			~MUCController();
			boost::signal<void ()> onUserLeft;
			virtual void setEnabled(bool enabled);
			void rejoin();
			static void appendToJoinParts(std::vector<NickJoinPart>& joinParts, const NickJoinPart& newEvent);
			static String generateJoinPartString(std::vector<NickJoinPart> joinParts);
		
		protected:
			void preSendMessageRequest(boost::shared_ptr<Message> message);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			String senderDisplayNameFromMessage(const JID& from);
			boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message> message) const;
			void preHandleIncomingMessage(boost::shared_ptr<MessageEvent>);

		private:
			void clearPresenceQueue();
			void addPresenceMessage(const String& message);
			void handleWindowClosed();
			void handleAvatarChanged(const JID& jid, const String&);
			void handleOccupantJoined(const MUCOccupant& occupant);
			void handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const String& reason);
			void handleOccupantPresenceChange(boost::shared_ptr<Presence> presence);
			void handleOccupantRoleChanged(const String& nick, const MUCOccupant& occupant,const MUCOccupant::Role& oldRole);
			void handleJoinComplete(const String& nick);
			void handleJoinFailed(boost::shared_ptr<ErrorPayload> error);
			void handleJoinTimeoutTick();
			String roleToGroupName(MUCOccupant::Role role);
			JID nickToJID(const String& nick);
			String roleToFriendlyName(MUCOccupant::Role role);
			void receivedActivity();
			bool messageTargetsMe(boost::shared_ptr<Message> message);
			void updateJoinParts();
			bool shouldUpdateJoinParts();
		private:
			MUC* muc_;
			UIEventStream* events_;
			String nick_;
			Roster* roster_;
			TabComplete* completer_;
			bool parting_;
			bool joined_;
			bool lastWasPresence_;
			boost::bsignals::scoped_connection avatarChangedConnection_;
			boost::shared_ptr<Timer> loginCheckTimer_;
			std::set<String> currentOccupants_;
			std::vector<NickJoinPart> joinParts_;
	};
}

