/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/SecurityLabelsCatalog.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Queries/IQRouter.h>

#include <Swift/Controllers/HighlightManager.h>
#include <Swift/Controllers/HistoryController.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>
#include <Swift/Controllers/XMPPEvents/MUCInviteEvent.h>
#include <Swift/Controllers/XMPPEvents/MessageEvent.h>

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

    class ChatControllerBase : public boost::signals2::trackable {
        public:
            virtual ~ChatControllerBase();
            void showChatWindow();
            void activateChatWindow();
            bool hasOpenWindow() const;
            virtual void setAvailableServerFeatures(std::shared_ptr<DiscoInfo> info);
            void handleIncomingMessage(std::shared_ptr<MessageEvent> message);
            std::string addMessage(const ChatWindow::ChatMessage& chatMessage, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::filesystem::path& avatarPath, const boost::posix_time::ptime& time);
            void replaceMessage(const ChatWindow::ChatMessage& chatMessage, const std::string& id, const boost::posix_time::ptime& time);
            virtual void setOnline(bool online);
            void setEnabled(bool enabled);
            virtual void setToJID(const JID& jid) {toJID_ = jid;}
            /** Used for determining when something is recent.*/
            boost::signals2::signal<void (const std::string& /*activity*/)> onActivity;
            boost::signals2::signal<void ()> onUnreadCountChanged;
            boost::signals2::signal<void ()> onWindowClosed;
            int getUnreadCount();
            const JID& getToJID() {return toJID_;}
            void handleCapsChanged(const JID& jid);
            void setCanStartImpromptuChats(bool supportsImpromptu);
            virtual ChatWindow* detachChatWindow();
            boost::signals2::signal<void(ChatWindow* /*window to reuse*/, const std::vector<JID>& /*invite people*/, const std::string& /*reason*/)> onConvertToMUC;

        protected:
            ChatControllerBase(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &toJID, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, std::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider);

            /**
             * Pass the Message appended, and the stanza used to send it.
             */
            virtual void postSendMessage(const std::string&, std::shared_ptr<Stanza>) {}
            virtual std::string senderDisplayNameFromMessage(const JID& from) = 0;
            virtual std::string senderHighlightNameFromMessage(const JID& from) = 0;
            virtual bool isIncomingMessageFromMe(std::shared_ptr<Message>) = 0;
            virtual void preHandleIncomingMessage(std::shared_ptr<MessageEvent>) {}
            virtual void addMessageHandleIncomingMessage(const JID& from, const ChatWindow::ChatMessage& message, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& time);
            virtual void postHandleIncomingMessage(std::shared_ptr<MessageEvent>, const ChatWindow::ChatMessage&) {}
            virtual void preSendMessageRequest(std::shared_ptr<Message>) {}
            virtual bool isFromContact(const JID& from);
            virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(std::shared_ptr<Message>) const = 0;
            virtual void dayTicked() {}
            virtual void handleBareJIDCapsChanged(const JID& jid) = 0;
            std::string getErrorMessage(std::shared_ptr<ErrorPayload>);
            virtual void setContactIsReceivingPresence(bool /* isReceivingPresence */) {}
            virtual void cancelReplaces() = 0;
            /** JID any iq for account should go to - bare except for PMs */
            virtual JID getBaseJID();
            virtual void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming) = 0;
            ChatWindow::ChatMessage buildChatWindowChatMessage(const std::string& message, bool senderIsSelf, const HighlightAction& fullMessageHighlightAction);
            void handleHighlightActions(const ChatWindow::ChatMessage& chatMessage);
            void updateMessageCount();

        private:
            IDGenerator idGenerator_;
            std::string lastSentMessageStanzaID_;
            void createDayChangeTimer();

            void handleSendMessageRequest(const std::string &body, bool isCorrectionMessage);
            void handleAllMessagesRead();
            void handleSecurityLabelsCatalogResponse(std::shared_ptr<SecurityLabelsCatalog>, ErrorPayload::ref error);
            void handleDayChangeTick();
            void handleMUCInvitation(Message::ref message);
            void handleMediatedMUCInvitation(Message::ref message);
            void handleGeneralMUCInvitation(MUCInviteEvent::ref event);
            void handleLogCleared();

        protected:
            JID selfJID_;
            std::vector<std::shared_ptr<StanzaEvent> > unreadMessages_;
            std::vector<std::shared_ptr<StanzaEvent> > targetedUnreadMessages_;
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
            std::shared_ptr<Timer> dateChangeTimer_;
            TimerFactory* timerFactory_;
            EntityCapsProvider* entityCapsProvider_;
            SecurityLabelsCatalog::Item lastLabel_;
            HistoryController* historyController_;
            MUCRegistry* mucRegistry_;
            Highlighter* highlighter_;
            std::shared_ptr<ChatMessageParser> chatMessageParser_;
            AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider_;
            UIEventStream* eventStream_;
            bool lastWasPresence_ = false;
    };
}
