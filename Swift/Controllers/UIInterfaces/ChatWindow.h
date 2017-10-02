/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/Tristate.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/SecurityLabelsCatalog.h>
#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/Highlighting/HighlightManager.h>

namespace Swift {
    class AvatarManager;
    class TreeWidget;
    class Roster;
    class TabComplete;
    class RosterItem;
    class ContactRosterItem;
    class FileTransferController;
    class UserSearchWindow;


    class ChatWindow {
        public:
            class ChatMessagePart {
                public:
                    virtual ~ChatMessagePart() {}
            };

            class ChatMessage {
                public:
                    ChatMessage() {}

                    ChatMessage(const std::string& text) {
                        append(std::make_shared<ChatTextMessagePart>(text));
                    }

                    void append(const std::shared_ptr<ChatMessagePart>& part) {
                        parts_.push_back(part);
                    }

                    const std::vector<std::shared_ptr<ChatMessagePart> >& getParts() const {
                        return parts_;
                    }

                    void setParts(const std::vector<std::shared_ptr<ChatMessagePart> >& parts) {
                        parts_ = parts;
                    }

                    void setHighlightActionSender(const HighlightAction& action) {
                        highlightActionSender_ = action;
                    }

                    const HighlightAction& getHighlightActionSender() const {
                        return highlightActionSender_;
                    }

                    void setHighlightActionOwnMention(const HighlightAction& action) {
                        highlightActionOwnMention_ = action;
                    }

                    const HighlightAction& getHighlightActionOwnMention() const {
                        return highlightActionOwnMention_;
                    }

                    void setHighlightActionGroupMessage(const HighlightAction& action) {
                        highlightActionGroupMessage_ = action;
                    }

                    const HighlightAction& getHighlightActionGroupMessage() const {
                        return highlightActionGroupMessage_;
                    }

                    void setHighlightActonDirectMessage(const HighlightAction& action) {
                        highlightActionDirectMessage_ = action;
                    }

                    const HighlightAction& getHighlightActionDirectMessage() const {
                        return highlightActionDirectMessage_;
                    }

                    bool isMeCommand() const {
                        return isMeCommand_;
                    }

                    void setIsMeCommand(bool isMeCommand) {
                        isMeCommand_ = isMeCommand;
                    }

                private:
                    std::vector<std::shared_ptr<ChatMessagePart> > parts_;
                    HighlightAction highlightActionSender_;
                    HighlightAction highlightActionOwnMention_;
                    HighlightAction highlightActionGroupMessage_;
                    HighlightAction highlightActionDirectMessage_;
                    bool isMeCommand_ = false;
            };

            class ChatTextMessagePart : public ChatMessagePart {
                public:
                    ChatTextMessagePart(const std::string& text) : text(text) {}
                    std::string text;
            };

            class ChatURIMessagePart : public ChatMessagePart {
                public:
                    ChatURIMessagePart(const std::string& target) : target(target) {}
                    std::string target;
            };

            class ChatEmoticonMessagePart : public ChatMessagePart {
                public:
                    std::string imagePath;
                    std::string alternativeText;
            };

            class ChatHighlightingMessagePart : public ChatMessagePart {
                public:
                    HighlightAction action;
                    std::string text;
            };


            enum AckState {Pending, Received, Failed};
            enum ReceiptState {ReceiptRequested, ReceiptReceived, ReceiptFailed};
            enum OccupantAction {Kick, Ban, MakeModerator, MakeParticipant, MakeVisitor, AddContact, ShowProfile};
            enum RoomAction {ChangeSubject, Configure, Affiliations, Destroy, Invite};
            enum FileTransferState {
                Initialisation,     ///< Collecting information required for sending the request out.
                WaitingForAccept,   ///< The file transfer request was send out.
                Negotiating,        ///< The other party accepted the file transfer request and a suitable transfer method is negotiated.
                Transferring,       ///< The negotiation was successful and the file is currently transferred.
                Canceled,           ///< Someone actively canceled the transfer.
                Finished,           ///< The file was transferred successfully.
                FTFailed            ///< The negotiation, the transfer itself or the verification failed.
            };
            enum WhiteboardSessionState {WhiteboardAccepted, WhiteboardTerminated, WhiteboardRejected};
            enum BlockingState {BlockingUnsupported, IsBlocked, IsUnblocked};
            enum Direction { UnknownDirection, DefaultDirection };
            enum MUCType { StandardMUC, ImpromptuMUC };
            enum TimestampBehaviour { KeepTimestamp, UpdateTimestamp };
            enum RoomBookmarkState { RoomNotBookmarked, RoomBookmarked, RoomAutoJoined };

            ChatWindow() {}
            virtual ~ChatWindow() {}

            /** Add message to window.
             * @return id of added message (for acks).
             */
            virtual std::string addMessage(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;
            /** Adds action to window.
             * @return id of added message (for acks);
             */
            virtual std::string addAction(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, std::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;

            /** Adds system message to window
             * @return id of added message (for replacement)
             */
            virtual std::string addSystemMessage(const ChatMessage& message, Direction direction) = 0;
            virtual void addPresenceMessage(const ChatMessage& message, Direction direction) = 0;

            virtual void addErrorMessage(const ChatMessage& message) = 0;
            virtual void replaceMessage(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) = 0;
            virtual void replaceSystemMessage(const ChatMessage& message, const std::string& id, const TimestampBehaviour timestampBehaviour) = 0;
            virtual void replaceWithAction(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time) = 0;

            // File transfer related stuff
            virtual std::string addFileTransfer(const std::string& senderName, const std::string& avatarPath, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes, const std::string& description) = 0;
            virtual void setFileTransferProgress(std::string, const int percentageDone) = 0;
            virtual void setFileTransferStatus(std::string, const FileTransferState state, const std::string& msg = "") = 0;
            virtual void addMUCInvitation(const std::string& senderName, const JID& jid, const std::string& reason, const std::string& password, bool direct = true, bool isImpromptu = false, bool isContinuation = false) = 0;

            virtual std::string addWhiteboardRequest(bool senderIsSelf) = 0;
            virtual void setWhiteboardSessionStatus(std::string id, const ChatWindow::WhiteboardSessionState state) = 0;

            // message receipts
            virtual void setMessageReceiptState(const std::string& id, ChatWindow::ReceiptState state) = 0;

            virtual void setContactChatState(ChatState::ChatStateType state) = 0;
            virtual void setName(const std::string& name) = 0;
            virtual void show() = 0;
            virtual bool isVisible() const = 0;
            virtual void activate() = 0;
            virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) = 0;
            virtual void setSecurityLabelsEnabled(bool enabled) = 0;
            virtual void setCorrectionEnabled(Tristate enabled) = 0;
            virtual void setFileTransferEnabled(Tristate enabled) = 0;
            virtual void setUnreadMessageCount(int count) = 0;
            virtual void convertToMUC(MUCType mucType) = 0;
//            virtual TreeWidget *getTreeWidget() = 0;
            virtual void setSecurityLabelsError() = 0;
            virtual SecurityLabelsCatalog::Item getSelectedSecurityLabel() = 0;
            virtual void setOnline(bool online) = 0;
            virtual void setRosterModel(Roster* model) = 0;
            virtual void setTabComplete(TabComplete* completer) = 0;
            virtual void replaceLastMessage(const ChatMessage& message, const TimestampBehaviour timestampBehaviour) = 0;
            virtual void setAckState(const std::string& id, AckState state) = 0;
            virtual void flash() = 0;
            virtual void setSubject(const std::string& subject) = 0;
            virtual void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&) = 0;
            virtual void setAvailableRoomActions(const std::vector<RoomAction> &actions) = 0;
            virtual void setBlockingState(BlockingState state) = 0;
            virtual void setCanInitiateImpromptuChats(bool supportsImpromptu) = 0;
            virtual void showBookmarkWindow(const MUCBookmark& bookmark) = 0;
            virtual void setBookmarkState(RoomBookmarkState bookmarkState) = 0;

            /**
             * A handle that uniquely identities an alert message.
             */
            typedef int AlertID;
            /**
             * Set an alert on the window.
             * @param alertText Description of alert (required).
             * @return A handle to the alert message.
             */
            virtual AlertID addAlert(const std::string& alertText) = 0;
            /**
             * Removes an alert.
             * @param id An alert ID previously returned from setAlert
             */
            virtual void removeAlert(const AlertID id) = 0;

            /**
             * Actions that can be performed on the selected occupant.
             */
            virtual void setAvailableOccupantActions(const std::vector<OccupantAction>& actions) = 0;

            /**
             * A room configuration has been requested, show the form.
             * If the form is cancelled, must emit onConfigurationFormCancelled().
             */
            virtual void showRoomConfigurationForm(Form::ref) = 0;

            boost::signals2::signal<void ()> onClosed;
            boost::signals2::signal<void ()> onAllMessagesRead;
            boost::signals2::signal<void (const std::string&, bool isCorrection)> onSendMessageRequest;
            boost::signals2::signal<void ()> onSendCorrectionMessageRequest;
            boost::signals2::signal<void ()> onUserTyping;
            boost::signals2::signal<void ()> onUserCancelsTyping;
            boost::signals2::signal<void ()> onAlertButtonClicked;
            boost::signals2::signal<void (ContactRosterItem*)> onOccupantSelectionChanged;
            boost::signals2::signal<void (ChatWindow::OccupantAction, ContactRosterItem*)> onOccupantActionSelected;
            boost::signals2::signal<void (const std::string&)> onChangeSubjectRequest;
            boost::signals2::signal<void ()> onBookmarkRequest;
            boost::signals2::signal<void (Form::ref)> onConfigureRequest;
            boost::signals2::signal<void ()> onDestroyRequest;
            boost::signals2::signal<void (const std::vector<JID>&)> onInviteToChat;
            boost::signals2::signal<void ()> onConfigurationFormCancelled;
            boost::signals2::signal<void ()> onGetAffiliationsRequest;
            boost::signals2::signal<void (MUCOccupant::Affiliation, const JID&)> onSetAffiliationRequest;
            boost::signals2::signal<void (const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& changes)> onChangeAffiliationsRequest;
            boost::signals2::signal<void ()> onContinuationsBroken;

            // File transfer related
            boost::signals2::signal<void (std::string /* id */)> onFileTransferCancel;
            boost::signals2::signal<void (std::string /* id */, std::string /* description */)> onFileTransferStart;
            boost::signals2::signal<void (std::string /* id */, std::string /* path */)> onFileTransferAccept;
            boost::signals2::signal<void (std::string /* path */)> onSendFileRequest;

            //Whiteboard related
            boost::signals2::signal<void ()> onWhiteboardSessionAccept;
            boost::signals2::signal<void ()> onWhiteboardSessionCancel;
            boost::signals2::signal<void ()> onWhiteboardWindowShow;

            // Blocking Command related
            boost::signals2::signal<void ()> onBlockUserRequest;
            boost::signals2::signal<void ()> onUnblockUserRequest;
    };
}

