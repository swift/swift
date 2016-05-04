/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/foreach.h>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
    class MockChatWindow : public ChatWindow {
        public:
            MockChatWindow() : labelsEnabled_(false), impromptuMUCSupported_(false) {}
            virtual ~MockChatWindow();

            virtual std::string addMessage(const ChatMessage& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/) {
                lastAddedMessage_ = message;
                return "id";
            }

            virtual std::string addAction(const ChatMessage& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, std::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime& /*time*/) {
                lastAddedAction_ = message;
                return "id";
            }

            virtual std::string addSystemMessage(const ChatMessage& message, Direction /*direction*/) {
                lastAddedSystemMessage_ = message;
                return "id";
            }

            virtual void addPresenceMessage(const ChatMessage& message, Direction /*direction*/) {
                lastAddedPresence_ = message;
            }

            virtual void addErrorMessage(const ChatMessage& /*message*/) {}
            virtual void replaceMessage(const ChatMessage& /*message*/, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/) {}
            virtual void replaceWithAction(const ChatMessage& /*message*/, const std::string& /*id*/, const boost::posix_time::ptime& /*time*/) {}
            virtual void replaceLastMessage(const ChatMessage& message, const TimestampBehaviour /*timestampBehaviour*/) {
                lastReplacedMessage_ = message;
            }
            virtual void replaceSystemMessage(const ChatMessage& /*message*/, const std::string& /*id*/, const TimestampBehaviour /*timestampBehaviour*/) {}

            // File transfer related stuff
            virtual std::string addFileTransfer(const std::string& /*senderName*/, bool /*senderIsSelf*/,const std::string& /*filename*/, const boost::uintmax_t /*sizeInBytes*/, const std::string& /*description*/) { return nullptr; }
            virtual void setFileTransferProgress(std::string /*id*/, const int /*alreadyTransferedBytes*/) { }
            virtual void setFileTransferStatus(std::string /*id*/, const FileTransferState /*state*/, const std::string& /*msg*/) { }

            virtual void setMessageReceiptState(const std::string &/* id */, ReceiptState /* state */) { }

            virtual void setContactChatState(ChatState::ChatStateType /*state*/) {}
            virtual void setName(const std::string& name) {name_ = name;}
            virtual void show() {}
            virtual bool isVisible() const { return true; }
            virtual void activate() {}
            virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {labels_ = labels;}
            virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;}
            virtual void setUnreadMessageCount(int /*count*/) {}
            virtual void convertToMUC(MUCType /*mucType*/) {}
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
            virtual void addMUCInvitation(const std::string& /*senderName*/, const JID& /*jid*/, const std::string& /*reason*/, const std::string& /*password*/, bool = true, bool = false, bool = false) {}

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
                foreach (std::shared_ptr<ChatMessagePart> part, message.getParts()) {
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
                lastAddedMessage_ = lastAddedAction_ = lastAddedPresence_ = lastReplacedMessage_ = lastAddedSystemMessage_ = ChatMessage();
            }

            std::string name_;
            ChatMessage lastAddedMessage_;
            ChatMessage lastAddedAction_;
            ChatMessage lastAddedPresence_;
            ChatMessage lastReplacedMessage_;
            ChatMessage lastAddedSystemMessage_;
            std::vector<SecurityLabelsCatalog::Item> labels_;
            bool labelsEnabled_;
            bool impromptuMUCSupported_;
            SecurityLabelsCatalog::Item label_;
            Roster* roster_;
    };
}

