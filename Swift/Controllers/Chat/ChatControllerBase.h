/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Swiften/Network/Timer.h"
#include "Swiften/Network/TimerFactory.h"
#include "Swiften/Elements/Stanza.h"
#include <string>
#include "Swiften/Elements/DiscoInfo.h"
#include "Swift/Controllers/XMPPEvents/MessageEvent.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/SecurityLabelsCatalog.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Base/IDGenerator.h"

namespace Swift {
	class IQRouter;
	class StanzaChannel;
	class ChatWindow;
	class ChatWindowFactory;
	class AvatarManager;
	class UIEventStream;
	class EventController;
	class EntityCapsProvider;

	class ChatControllerBase : public boost::bsignals::trackable {
		public:
			virtual ~ChatControllerBase();
			void showChatWindow();
			void activateChatWindow();
			void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<MessageEvent> message);
			std::string addMessage(const std::string& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time);
			virtual void setOnline(bool online);
			virtual void setEnabled(bool enabled);
			virtual void setToJID(const JID& jid) {toJID_ = jid;};
			/** Used for determining when something is recent.*/
			boost::signal<void (const std::string& /*activity*/)> onActivity;
			boost::signal<void ()> onUnreadCountChanged;
			int getUnreadCount();
			const JID& getToJID() {return toJID_;}
			void handleCapsChanged(const JID& jid);

		protected:
			ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider);

			/**
			 * Pass the Message appended, and the stanza used to send it.
			 */
			virtual void postSendMessage(const std::string&, boost::shared_ptr<Stanza>) {};
			virtual std::string senderDisplayNameFromMessage(const JID& from) = 0;
			virtual bool isIncomingMessageFromMe(boost::shared_ptr<Message>) = 0;
			virtual void preHandleIncomingMessage(boost::shared_ptr<MessageEvent>) {};
			virtual void postHandleIncomingMessage(boost::shared_ptr<MessageEvent>) {};
			virtual void preSendMessageRequest(boost::shared_ptr<Message>) {};
			virtual bool isFromContact(const JID& from);
			virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message>) const = 0;
			virtual void dayTicked() {};
			virtual void handleBareJIDCapsChanged(const JID& jid) = 0;
			std::string getErrorMessage(boost::shared_ptr<ErrorPayload>);
			virtual void setContactIsReceivingPresence(bool /* isReceivingPresence */) {}

		private:
			IDGenerator idGenerator_;
			std::string lastSentMessageStanzaID_;
			void createDayChangeTimer();
			void handleSendMessageRequest(const std::string &body, bool isCorrectionMessage);
			void handleAllMessagesRead();
			void handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog>, ErrorPayload::ref error);
			void handleDayChangeTick();
			void handleMUCInvitation(Message::ref message);
			void handleMediatedMUCInvitation(Message::ref message);

		protected:
			JID selfJID_;
			std::vector<boost::shared_ptr<MessageEvent> > unreadMessages_;
			StanzaChannel* stanzaChannel_;
			IQRouter* iqRouter_;
			ChatWindowFactory* chatWindowFactory_;
			ChatWindow* chatWindow_;
			JID toJID_;
			bool labelsEnabled_;
			std::map<JID, std::string> lastMessagesUIID_;
			PresenceOracle* presenceOracle_;
			AvatarManager* avatarManager_;
			bool useDelayForLatency_;
			EventController* eventController_;
			boost::shared_ptr<Timer> dateChangeTimer_;
			TimerFactory* timerFactory_;
			EntityCapsProvider* entityCapsProvider_;
	};
}
