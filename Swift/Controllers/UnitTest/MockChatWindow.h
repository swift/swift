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

			virtual void addMessage(const String& message, const String& /*senderName*/, bool /*senderIsSelf*/, const boost::optional<SecurityLabel>& /*label*/, const String& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message;};
			virtual void addAction(const String& message, const String& /*senderName*/, bool /*senderIsSelf*/, const boost::optional<SecurityLabel>& /*label*/, const String& /*avatarPath*/, const boost::posix_time::ptime&) {lastMessageBody_ = message;};
			virtual void addSystemMessage(const String& /*message*/) {};
			virtual void addErrorMessage(const String& /*message*/) {};

			virtual void setContactChatState(ChatState::ChatStateType /*state*/) {};
			virtual void setName(const String& name) {name_ = name;};
			virtual void show() {};
			virtual void activate() {};
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabel>& labels) {labels_ = labels;};
			virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;};
			virtual void setUnreadMessageCount(int /*count*/) {};
			virtual void convertToMUC() {};
			virtual void setSecurityLabelsError() {};
			virtual SecurityLabel getSelectedSecurityLabel() {return SecurityLabel();};
			virtual void setInputEnabled(bool /*enabled*/) {};
			virtual void setRosterModel(Roster* /*roster*/) {};
			virtual void setTabComplete(TabComplete*) {};

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const String&)> onSendMessageRequest;

			String name_;
			String lastMessageBody_;
			std::vector<SecurityLabel> labels_;
			bool labelsEnabled_;
	};
}

