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

#include <string>
#include "Swiften/Network/Timer.h"
#include "Swift/Controllers/Chat/ChatControllerBase.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/DiscoInfo.h"
#include "Swiften/JID/JID.h"
#include "Swiften/MUC/MUC.h"
#include "Swiften/Elements/MUCOccupant.h"

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
			NickJoinPart(const std::string& nick, JoinPart type) : nick(nick), type(type) {};
			std::string nick;
			JoinPart type;
	};

	class MUCController : public ChatControllerBase {
		public:
			MUCController(const JID& self, MUC::ref muc, const std::string &nick, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, PresenceOracle* presenceOracle, AvatarManager* avatarManager, UIEventStream* events, bool useDelayForLatency, TimerFactory* timerFactory, EventController* eventController);
			~MUCController();
			boost::signal<void ()> onUserLeft;
			virtual void setOnline(bool online);
			void rejoin();
			static void appendToJoinParts(std::vector<NickJoinPart>& joinParts, const NickJoinPart& newEvent);
			static std::string generateJoinPartString(const std::vector<NickJoinPart>& joinParts);
			static std::string concatenateListOfNames(const std::vector<NickJoinPart>& joinParts);
		
		protected:
			void preSendMessageRequest(boost::shared_ptr<Message> message);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			std::string senderDisplayNameFromMessage(const JID& from);
			boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message> message) const;
			void preHandleIncomingMessage(boost::shared_ptr<MessageEvent>);
			void postHandleIncomingMessage(boost::shared_ptr<MessageEvent>);

		private:
			void clearPresenceQueue();
			void addPresenceMessage(const std::string& message);
			void handleWindowClosed();
			void handleAvatarChanged(const JID& jid);
			void handleOccupantJoined(const MUCOccupant& occupant);
			void handleOccupantLeft(const MUCOccupant& occupant, MUC::LeavingType type, const std::string& reason);
			void handleOccupantPresenceChange(boost::shared_ptr<Presence> presence);
			void handleOccupantRoleChanged(const std::string& nick, const MUCOccupant& occupant,const MUCOccupant::Role& oldRole);
			void handleJoinComplete(const std::string& nick);
			void handleJoinFailed(boost::shared_ptr<ErrorPayload> error);
			void handleJoinTimeoutTick();
			std::string roleToGroupName(MUCOccupant::Role role);
			JID nickToJID(const std::string& nick);
			std::string roleToFriendlyName(MUCOccupant::Role role);
			void receivedActivity();
			bool messageTargetsMe(boost::shared_ptr<Message> message);
			void updateJoinParts();
			bool shouldUpdateJoinParts();
			void dayTicked() {lastWasPresence_ = false;}
			void processUserPart();

		private:
			MUC::ref muc_;
			UIEventStream* events_;
			std::string nick_;
			std::string desiredNick_;
			Roster* roster_;
			TabComplete* completer_;
			bool parting_;
			bool joined_;
			bool lastWasPresence_;
			bool shouldJoinOnReconnect_;
			bool doneGettingHistory_;
			boost::bsignals::scoped_connection avatarChangedConnection_;
			boost::shared_ptr<Timer> loginCheckTimer_;
			std::set<std::string> currentOccupants_;
			std::vector<NickJoinPart> joinParts_;
			boost::posix_time::ptime lastActivity_;
	};
}

