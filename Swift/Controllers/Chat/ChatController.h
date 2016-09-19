/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <Swiften/Base/Override.h>
#include <Swiften/Base/Tristate.h>

#include <Swift/Controllers/Chat/ChatControllerBase.h>
#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
    class AvatarManager;
    class ChatStateNotifier;
    class ChatStateTracker;
    class ClientBlockListManager;
    class EntityCapsProvider;
    class FileTransferController;
    class HighlightManager;
    class HistoryController;
    class NickResolver;
    class SettingsProvider;
    class TimerFactory;
    class UIEvent;

    class ChatController : public ChatControllerBase {
        public:
            ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, TimerFactory* timerFactory, EventController* eventController, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, SettingsProvider* settings, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, std::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider);
            virtual ~ChatController();
            virtual void setToJID(const JID& jid) SWIFTEN_OVERRIDE;
            virtual void setAvailableServerFeatures(std::shared_ptr<DiscoInfo> info) SWIFTEN_OVERRIDE;
            virtual void setOnline(bool online) SWIFTEN_OVERRIDE;
            virtual void handleNewFileTransferController(FileTransferController* ftc);
            virtual void handleWhiteboardSessionRequest(bool senderIsSelf);
            virtual void handleWhiteboardStateChange(const ChatWindow::WhiteboardSessionState state);
            virtual void setContactIsReceivingPresence(bool /*isReceivingPresence*/) SWIFTEN_OVERRIDE;
            virtual ChatWindow* detachChatWindow() SWIFTEN_OVERRIDE;
            virtual void handleIncomingOwnMessage(std::shared_ptr<Message> message) SWIFTEN_OVERRIDE;

        protected:
            virtual void cancelReplaces() SWIFTEN_OVERRIDE;
            virtual JID getBaseJID() SWIFTEN_OVERRIDE;
            virtual void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming) SWIFTEN_OVERRIDE;
            virtual bool shouldIgnoreMessage(std::shared_ptr<Message> message) SWIFTEN_OVERRIDE;
            virtual JID messageCorrectionJID(const JID& fromJID) SWIFTEN_OVERRIDE;

        private:
            void handlePresenceChange(std::shared_ptr<Presence> newPresence);
            std::string getStatusChangeString(std::shared_ptr<Presence> presence);
            virtual bool isIncomingMessageFromMe(std::shared_ptr<Message> message) SWIFTEN_OVERRIDE;
            virtual void postSendMessage(const std::string &body, std::shared_ptr<Stanza> sentStanza) SWIFTEN_OVERRIDE;
            virtual void preHandleIncomingMessage(std::shared_ptr<MessageEvent> messageEvent) SWIFTEN_OVERRIDE;
            virtual void addMessageHandleIncomingMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& timeStamp) SWIFTEN_OVERRIDE;
            virtual void handleIncomingReplaceMessage(const JID& from, const ChatWindow::ChatMessage& message, const std::string& messageID, const std::string& idToReplace, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const boost::posix_time::ptime& timeStamp) SWIFTEN_OVERRIDE;
            virtual void postHandleIncomingMessage(std::shared_ptr<MessageEvent> messageEvent, const ChatWindow::ChatMessage& chatMessage) SWIFTEN_OVERRIDE;
            virtual void preSendMessageRequest(std::shared_ptr<Message>) SWIFTEN_OVERRIDE;
            virtual std::string senderHighlightNameFromMessage(const JID& from) SWIFTEN_OVERRIDE;
            virtual std::string senderDisplayNameFromMessage(const JID& from) SWIFTEN_OVERRIDE;
            virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(std::shared_ptr<Message>) const SWIFTEN_OVERRIDE;
            void handleStanzaAcked(std::shared_ptr<Stanza> stanza);
            virtual void dayTicked() SWIFTEN_OVERRIDE { lastWasPresence_ = false; }
            void handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/);
            virtual void handleBareJIDCapsChanged(const JID& jid) SWIFTEN_OVERRIDE;

            void handleFileTransferCancel(std::string /* id */);
            void handleFileTransferStart(std::string /* id */, std::string /* description */);
            void handleFileTransferAccept(std::string /* id */, std::string /* filename */);
            void handleSendFileRequest(std::string filename);

            void handleWhiteboardSessionAccept();
            void handleWhiteboardSessionCancel();
            void handleWhiteboardWindowShow();

            void handleSettingChanged(const std::string& settingPath);
            void checkForDisplayingDisplayReceiptsAlert();

            void handleBlockingStateChanged();
            void handleBlockUserRequest();
            void handleUnblockUserRequest();

            void handleInviteToChat(const std::vector<JID>& droppedJIDs);

            void handleWindowClosed();

            void handleUIEvent(std::shared_ptr<UIEvent> event);

        private:
            NickResolver* nickResolver_;
            ChatStateNotifier* chatStateNotifier_;
            ChatStateTracker* chatStateTracker_;
            std::string myLastMessageUIID_;
            bool isInMUC_;
            std::string lastStatusChangeString_;
            std::map<std::shared_ptr<Stanza>, std::string> unackedStanzas_;
            std::map<std::string, std::string> requestedReceipts_;
            StatusShow::Type lastShownStatus_;

            Tristate contactSupportsReceipts_;
            bool receivingPresenceFromUs_ = false;
            bool userWantsReceipts_;
            std::map<std::string, FileTransferController*> ftControllers;
            SettingsProvider* settings_;
            std::string lastWbID_;
            std::string lastHandledMessageID_;

            ClientBlockListManager* clientBlockListManager_;
            boost::signals2::scoped_connection blockingOnStateChangedConnection_;
            boost::signals2::scoped_connection blockingOnItemAddedConnection_;
            boost::signals2::scoped_connection blockingOnItemRemovedConnection_;

            boost::optional<ChatWindow::AlertID> deliveryReceiptAlert_;
            boost::optional<ChatWindow::AlertID> blockedContactAlert_;
    };
}

