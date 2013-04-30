/*
 * Copyright (c) 2010-2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/SecurityLabelsCatalog.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/MUC/MUCRegistry.h>

#include <Swift/Controllers/XMPPEvents/MessageEvent.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/HistoryController.h>
#include <Swift/Controllers/HighlightManager.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
	class IQRouter;
	class StanzaChannel;
	class ChatWindowFactory;
	class AvatarManager;
	class UIEventStream;
	class EventController;
	class EntityCapsProvider;
	class HighlightManager;
	class Highlighter;
	class ChatMessageParser;
	class AutoAcceptMUCInviteDecider;

	class ChatControllerBase : public boost::bsignals::trackable {
		public:
			virtual ~ChatControllerBase();
			void showChatWindow();
			void activateChatWindow();
			virtual void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			void handleIncomingMessage(boost::shared_ptr<MessageEvent> message);
			std::string addMessage(const std::string& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const boost::filesystem::path& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			void replaceMessage(const std::string& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight);
			virtual void setOnline(bool online);
			virtual void setEnabled(bool enabled);
			virtual void setToJID(const JID& jid) {toJID_ = jid;}
			/** Used for determining when something is recent.*/
			boost::signal<void (const std::string& /*activity*/)> onActivity;
			boost::signal<void ()> onUnreadCountChanged;
			int getUnreadCount();
			const JID& getToJID() {return toJID_;}
			void handleCapsChanged(const JID& jid);
			void setCanStartImpromptuChats(bool supportsImpromptu);
			virtual ChatWindow* detachChatWindow();
			boost::signal<void(ChatWindow* /*window to reuse*/, const std::vector<JID>& /*invite people*/, const std::string& /*reason*/)> onConvertToMUC;

		protected:
			ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ChatMessageParser* chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider);

			/**
			 * Pass the Message appended, and the stanza used to send it.
			 */
			virtual void postSendMessage(const std::string&, boost::shared_ptr<Stanza>) {}
			virtual std::string senderDisplayNameFromMessage(const JID& from) = 0;
			virtual bool isIncomingMessageFromMe(boost::shared_ptr<Message>) = 0;
			virtual void preHandleIncomingMessage(boost::shared_ptr<MessageEvent>) {}
			virtual void postHandleIncomingMessage(boost::shared_ptr<MessageEvent>, const HighlightAction&) {}
			virtual void preSendMessageRequest(boost::shared_ptr<Message>) {}
			virtual bool isFromContact(const JID& from);
			virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message>) const = 0;
			virtual void dayTicked() {}
			virtual void handleBareJIDCapsChanged(const JID& jid) = 0;
			std::string getErrorMessage(boost::shared_ptr<ErrorPayload>);
			virtual void setContactIsReceivingPresence(bool /* isReceivingPresence */) {}
			virtual void cancelReplaces() = 0;
			/** JID any iq for account should go to - bare except for PMs */
			virtual JID getBaseJID();
			virtual void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming) = 0;

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
			void handleGeneralMUCInvitation(MUCInviteEvent::ref event);
			void handleLogCleared();

		protected:
			JID selfJID_;
			std::vector<boost::shared_ptr<StanzaEvent> > unreadMessages_;
			std::vector<boost::shared_ptr<StanzaEvent> > targetedUnreadMessages_;
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
			SecurityLabelsCatalog::Item lastLabel_; 
			HistoryController* historyController_;
			MUCRegistry* mucRegistry_;
			Highlighter* highlighter_;
			ChatMessageParser* chatMessageParser_;
			AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider_;
			UIEventStream* eventStream_;
	};
}
