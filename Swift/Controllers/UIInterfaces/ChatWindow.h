/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/Tristate.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/Elements/SecurityLabelsCatalog.h>
#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/Controllers/HighlightManager.h>

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
						append(boost::make_shared<ChatTextMessagePart>(text));
					}
					void append(const boost::shared_ptr<ChatMessagePart>& part) {
						parts_.push_back(part);
					}

					const std::vector<boost::shared_ptr<ChatMessagePart> >& getParts() const {
						return parts_;
					}
				private:
					std::vector<boost::shared_ptr<ChatMessagePart> > parts_;
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
					std::string foregroundColor;
					std::string backgroundColor;
					std::string text;
			};


			enum AckState {Pending, Received, Failed};
			enum ReceiptState {ReceiptRequested, ReceiptReceived, ReceiptFailed};
			enum OccupantAction {Kick, Ban, MakeModerator, MakeParticipant, MakeVisitor, AddContact, ShowProfile};
			enum RoomAction {ChangeSubject, Configure, Affiliations, Destroy, Invite};
			enum FileTransferState {WaitingForAccept, Negotiating, Transferring, Canceled, Finished, FTFailed};
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
			virtual std::string addMessage(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) = 0;
			/** Adds action to window.
			 * @return id of added message (for acks);
			 */
			virtual std::string addAction(const ChatMessage& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time, const HighlightAction& highlight) = 0;

			/** Adds system message to window
			 * @return id of added message (for replacement)
			 */
			virtual std::string addSystemMessage(const ChatMessage& message, Direction direction) = 0;
			virtual void addPresenceMessage(const ChatMessage& message, Direction direction) = 0;

			virtual void addErrorMessage(const ChatMessage& message) = 0;
			virtual void replaceMessage(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) = 0;
			virtual void replaceSystemMessage(const ChatMessage& message, const std::string& id, const TimestampBehaviour timestampBehaviour) = 0;
			virtual void replaceWithAction(const ChatMessage& message, const std::string& id, const boost::posix_time::ptime& time, const HighlightAction& highlight) = 0;
			
			// File transfer related stuff
			virtual std::string addFileTransfer(const std::string& senderName, bool senderIsSelf, const std::string& filename, const boost::uintmax_t sizeInBytes) = 0;
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
//			virtual TreeWidget *getTreeWidget() = 0;
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
			 * @param buttonText Button text to use (optional, no button is shown if empty).
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

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const std::string&, bool isCorrection)> onSendMessageRequest;
			boost::signal<void ()> onSendCorrectionMessageRequest;
			boost::signal<void ()> onUserTyping;
			boost::signal<void ()> onUserCancelsTyping;
			boost::signal<void ()> onAlertButtonClicked;
			boost::signal<void (ContactRosterItem*)> onOccupantSelectionChanged;
			boost::signal<void (ChatWindow::OccupantAction, ContactRosterItem*)> onOccupantActionSelected;
			boost::signal<void (const std::string&)> onChangeSubjectRequest;
			boost::signal<void ()> onBookmarkRequest;
			boost::signal<void (Form::ref)> onConfigureRequest;
			boost::signal<void ()> onDestroyRequest;
			boost::signal<void (const std::vector<JID>&)> onInviteToChat;
			boost::signal<void ()> onConfigurationFormCancelled;
			boost::signal<void ()> onGetAffiliationsRequest;
			boost::signal<void (MUCOccupant::Affiliation, const JID&)> onSetAffiliationRequest;
			boost::signal<void (const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& changes)> onChangeAffiliationsRequest;
			boost::signal<void ()> onLogCleared;
			
			// File transfer related
			boost::signal<void (std::string /* id */)> onFileTransferCancel;
			boost::signal<void (std::string /* id */, std::string /* description */)> onFileTransferStart;
			boost::signal<void (std::string /* id */, std::string /* path */)> onFileTransferAccept;
			boost::signal<void (std::string /* path */)> onSendFileRequest;

			//Whiteboard related	
			boost::signal<void ()> onWhiteboardSessionAccept;
			boost::signal<void ()> onWhiteboardSessionCancel;
			boost::signal<void ()> onWhiteboardWindowShow;

			// Blocking Command related
			boost::signal<void ()> onBlockUserRequest;
			boost::signal<void ()> onUnblockUserRequest;
	};
}

