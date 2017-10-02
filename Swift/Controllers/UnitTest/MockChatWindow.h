/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
    class MockChatWindow : public ChatWindow {
        public:
            MockChatWindow() {}
            virtual ~MockChatWindow();

            virtual std::string addMessage(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/) {
                lastAddedMessage_ = message;
                lastAddedMessageSenderName_ = senderName;
                lastAddedMessageSenderIsSelf_ = senderIsSelf;
                return "id";
            }

            virtual std::string addAction(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/) {
                lastAddedAction_ = message;
                lastAddedActionSenderName_ = senderName;
                lastAddedActionSenderIsSelf_ = senderIsSelf;
                return "id";
            }

            virtual std::string addSystemMessage(const ChatMessage& message, Direction /*direction*/) {
                lastAddedSystemMessage_ = message;
                return "id";
            }

            virtual void addPresenceMessage(const ChatMessage& message, Direction /*direction*/) {
                lastAddedPresence_ = message;
            }

            virtual void addErrorMessage(const ChatMessage& message) {
                lastAddedErrorMessage_ = message;
            }

            virtual void replaceMessage(const ChatMessage& message, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/) {
                lastReplacedMessage_ = message;
            }

            virtual void replaceWithAction(const ChatMessage& /*message*/, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/) {}
            virtual void replaceLastMessage(const ChatMessage& message, const TimestampBehaviour /*timestampBehaviour*/) {
                lastReplacedLastMessage_ = message;
            }
            virtual void replaceSystemMessage(const ChatMessage& message, const std::string& /*id*/, const TimestampBehaviour /*timestampBehaviour*/) {
                lastReplacedSystemMessage_ = message;
            }

            // File transfer related stuff
            virtual std::string addFileTransfer(const std::string& /*senderName*/, const std::string& /*avatarPath*/, bool /*senderIsSelf*/,const std::string& /*filename*/, const boost::uintmax_t /*sizeInBytes*/, const std::string& /*description*/) { return nullptr; }
            virtual void setFileTransferProgress(std::string /*id*/, const int /*alreadyTransferedBytes*/) { }
            virtual void setFileTransferStatus(std::string /*id*/, const FileTransferState /*state*/, const std::string& /*msg*/) { }

            virtual void setMessageReceiptState(const std::string & id, ReceiptState state) {
                receiptChanges_.emplace_back(id, state);
            }

            virtual void setContactChatState(ChatState::ChatStateType /*state*/) {}
            virtual void setName(const std::string& name) {name_ = name;}
            virtual void show() {}
            virtual bool isVisible() const { return true; }
            virtual void activate() {}
            virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {labels_ = labels;}
            virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;}
            virtual void setUnreadMessageCount(int /*count*/) {}

            virtual void convertToMUC(MUCType mucType) {
                mucType_ = mucType;
            }

            virtual void setSecurityLabelsError() {}
            virtual SecurityLabelsCatalog::Item getSelectedSecurityLabel() {return label_;}
            virtual void setOnline(bool /*online*/) {}
            virtual void setRosterModel(Roster* roster) { roster_ = roster; }
            Roster* getRosterModel() { return roster_; }
            virtual void setTabComplete(TabComplete*) {}

            void setAckState(const std::string& /*id*/, AckState /*state*/) {}
            virtual void flash() {}
            virtual AlertID addAlert(const std::string& /*alertText*/) { return 0; }
            virtual void removeAlert(const AlertID /*id*/) {}
            virtual void setCorrectionEnabled(Tristate /*enabled*/) {}
            virtual void setFileTransferEnabled(Tristate /*enabled*/) {}
            void setAvailableOccupantActions(const std::vector<OccupantAction>&/* actions*/) {}
            void setSubject(const std::string& /*subject*/) {}
            virtual void showRoomConfigurationForm(Form::ref) {}
            virtual void addMUCInvitation(const std::string& /*senderName*/, const JID& jid, const std::string& /*reason*/, const std::string& /*password*/, bool = true, bool = false, bool = false) {
                lastMUCInvitationJID_ = jid;
            }

            virtual std::string addWhiteboardRequest(bool) {return "";}
            virtual void setWhiteboardSessionStatus(std::string, const ChatWindow::WhiteboardSessionState){}

            virtual void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&) {}
            virtual void setAvailableRoomActions(const std::vector<RoomAction> &) {}

            virtual void setBlockingState(BlockingState) {}
            virtual void setCanInitiateImpromptuChats(bool supportsImpromptu) {
                impromptuMUCSupported_ = supportsImpromptu;
            }

            virtual void showBookmarkWindow(const MUCBookmark& /*bookmark*/) {}
            virtual void setBookmarkState(RoomBookmarkState) {}

            static std::string bodyFromMessage(const ChatMessage& message) {
                std::string body;
                std::shared_ptr<ChatTextMessagePart> text;
                std::shared_ptr<ChatHighlightingMessagePart> highlight;
                for (auto &&part : message.getParts()) {
                    if ((text = std::dynamic_pointer_cast<ChatTextMessagePart>(part))) {
                        body += text->text;
                    }
                    else if ((highlight = std::dynamic_pointer_cast<ChatHighlightingMessagePart>(part))) {
                        body += highlight->text;
                    }
                }
                return body;
            }

            void resetLastMessages() {
                lastAddedMessage_ = lastAddedAction_ = lastAddedPresence_ = lastReplacedLastMessage_ = lastAddedSystemMessage_ = lastReplacedSystemMessage_ = ChatMessage();
                lastAddedMessageSenderName_ = lastAddedActionSenderName_ = "";
                lastAddedMessageSenderIsSelf_ = lastAddedActionSenderIsSelf_ = false;
            }

            std::string name_;
            ChatMessage lastAddedMessage_;
            std::string lastAddedMessageSenderName_;
            bool lastAddedMessageSenderIsSelf_ = false;
            ChatMessage lastAddedAction_;
            std::string lastAddedActionSenderName_;
            bool lastAddedActionSenderIsSelf_ = false;
            ChatMessage lastAddedPresence_;
            ChatMessage lastReplacedMessage_;
            ChatMessage lastReplacedLastMessage_;
            ChatMessage lastAddedSystemMessage_;
            ChatMessage lastReplacedSystemMessage_;
            ChatMessage lastAddedErrorMessage_;
            JID lastMUCInvitationJID_;
            std::vector<SecurityLabelsCatalog::Item> labels_;
            bool labelsEnabled_ = false;
            bool impromptuMUCSupported_ = false;
            SecurityLabelsCatalog::Item label_;
            Roster* roster_ = nullptr;
            std::vector<std::pair<std::string, ReceiptState>> receiptChanges_;
            boost::optional<MUCType> mucType_;
    };
}

