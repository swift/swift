/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swift/Controllers/UIInterfaces/ChatWindow.h"

namespace Swift {
	class MockChatWindow : public ChatWindow {
		public:
			MockChatWindow() : labelsEnabled_(false) {};
			virtual ~MockChatWindow();

			virtual std::string addMessage(const std::string& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, boost::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message; return "";};
			virtual std::string addAction(const std::string& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, boost::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message; return "";};
			virtual void addSystemMessage(const std::string& /*message*/) {};
			virtual void addErrorMessage(const std::string& /*message*/) {};
			virtual void addPresenceMessage(const std::string& /*message*/) {};

			// File transfer related stuff
			virtual std::string addFileTransfer(const std::string& /*senderName*/, bool /*senderIsSelf*/,const std::string& /*filename*/, const boost::uintmax_t /*sizeInBytes*/) { return 0; };
			virtual void setFileTransferProgress(std::string /*id*/, const int /*alreadyTransferedBytes*/) { };
			virtual void setFileTransferStatus(std::string /*id*/, const FileTransferState /*state*/, const std::string& /*msg*/) { };
			
			virtual void setMessageReceiptState(const std::string &/* id */, ReceiptState /* state */) { }

			virtual void setContactChatState(ChatState::ChatStateType /*state*/) {};
			virtual void setName(const std::string& name) {name_ = name;};
			virtual void show() {};
			virtual void activate() {};
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {labels_ = labels;};
			virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;};
			virtual void setUnreadMessageCount(int /*count*/) {};
			virtual void convertToMUC() {};
			virtual void setSecurityLabelsError() {};
			virtual SecurityLabelsCatalog::Item getSelectedSecurityLabel() {return label_;}
			virtual void setInputEnabled(bool /*enabled*/) {};
			virtual void setRosterModel(Roster* /*roster*/) {};
			virtual void setTabComplete(TabComplete*) {};
			virtual void replaceLastMessage(const std::string&) {};
			virtual void replaceMessage(const std::string&, const std::string&, const boost::posix_time::ptime&) {};
			void setAckState(const std::string& /*id*/, AckState /*state*/) {};
			virtual void flash() {};
			virtual void setAlert(const std::string& /*alertText*/, const std::string& /*buttonText*/) {};
			virtual void cancelAlert() {};
			virtual void setCorrectionEnabled(Tristate /*enabled*/) {}
			void setAvailableOccupantActions(const std::vector<OccupantAction>&/* actions*/) {}
			void setSubject(const std::string& /*subject*/) {}
			virtual void showRoomConfigurationForm(Form::ref) {}
			virtual void addMUCInvitation(const JID& /*jid*/, const std::string& /*reason*/, const std::string& /*password*/, bool = true) {};
			virtual void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>&) {}

			std::string name_;
			std::string lastMessageBody_;
			std::vector<SecurityLabelsCatalog::Item> labels_;
			bool labelsEnabled_;
			SecurityLabelsCatalog::Item label_;
	};
}

