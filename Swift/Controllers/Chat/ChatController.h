/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/Chat/ChatControllerBase.h"

namespace Swift {
	class AvatarManager;
	class ChatStateNotifier;
	class ChatStateTracker;
	class NickResolver;
	class EntityCapsProvider;

	class ChatController : public ChatControllerBase {
		public:
			ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider);
			virtual ~ChatController();
			virtual void setToJID(const JID& jid);
			virtual void setOnline(bool online);

		private:
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
			std::string getStatusChangeString(boost::shared_ptr<Presence> presence);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			void postSendMessage(const std::string &body, boost::shared_ptr<Stanza> sentStanza);
			void preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent);
			void postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent);
			void preSendMessageRequest(boost::shared_ptr<Message>);
			std::string senderDisplayNameFromMessage(const JID& from);
			virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message>) const;
			void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
			void dayTicked() {lastWasPresence_ = false;}
			void handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/);

		private:
			NickResolver* nickResolver_;
			ChatStateNotifier* chatStateNotifier_;
			ChatStateTracker* chatStateTracker_;
			bool isInMUC_;
			bool lastWasPresence_;
			std::string lastStatusChangeString_;
			std::map<boost::shared_ptr<Stanza>, std::string> unackedStanzas_;
			StatusShow::Type lastShownStatus_;
	};
}

