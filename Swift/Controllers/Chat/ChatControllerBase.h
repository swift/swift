/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_ChatControllerBase_H
#define SWIFTEN_ChatControllerBase_H

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

namespace Swift {
	class IQRouter;
	class StanzaChannel;
	class ChatWindow;
	class ChatWindowFactory;
	class AvatarManager;
	class UIEventStream;
	class EventController;

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
		protected:
			ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory);

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

		private:
			void createDayChangeTimer();
			void handleSendMessageRequest(const std::string &body);
			void handleAllMessagesRead();
			void handleSecurityLabelsCatalogResponse(boost::shared_ptr<SecurityLabelsCatalog>, ErrorPayload::ref error);
			std::string getErrorMessage(boost::shared_ptr<ErrorPayload>);
			void handleDayChangeTick();

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
			bool useDelayForLatency_;
			EventController* eventController_;
			boost::shared_ptr<Timer> dateChangeTimer_;
			TimerFactory* timerFactory_;
	};
}

#endif
