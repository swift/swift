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
			MockChatWindow() {};
			virtual ~MockChatWindow();

			virtual std::string addMessage(const std::string& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, boost::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message; return "";};
			virtual std::string addAction(const std::string& message, const std::string& /*senderName*/, bool /*senderIsSelf*/, boost::shared_ptr<SecurityLabel> /*label*/, const std::string& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message; return "";};
			virtual void addSystemMessage(const std::string& /*message*/) {};
			virtual void addErrorMessage(const std::string& /*message*/) {};
			virtual void addPresenceMessage(const std::string& /*message*/) {};

			virtual void setContactChatState(ChatState::ChatStateType /*state*/) {};
			virtual void setName(const std::string& name) {name_ = name;};
			virtual void show() {};
			virtual void activate() {};
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) {labels_ = labels;};
			virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;};
			virtual void setUnreadMessageCount(int /*count*/) {};
			virtual void convertToMUC() {};
			virtual void setSecurityLabelsError() {};
			virtual SecurityLabelsCatalog::Item getSelectedSecurityLabel() {return SecurityLabelsCatalog::Item();};
			virtual void setInputEnabled(bool /*enabled*/) {};
			virtual void setRosterModel(Roster* /*roster*/) {};
			virtual void setTabComplete(TabComplete*) {};
			virtual void replaceLastMessage(const std::string&) {};
			void setAckState(const std::string& /*id*/, AckState /*state*/) {};
			virtual void flash() {};

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const std::string&)> onSendMessageRequest;

			std::string name_;
			std::string lastMessageBody_;
			std::vector<SecurityLabelsCatalog::Item> labels_;
			bool labelsEnabled_;
	};
}

