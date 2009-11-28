#pragma once

#include "Swift/Controllers/ChatWindow.h"

namespace Swift {
	class MockChatWindow : public ChatWindow {
		public:
			MockChatWindow() {};
			virtual ~MockChatWindow();

			virtual void addMessage(const String& /*message*/, const String& /*senderName*/, bool /*senderIsSelf*/, const boost::optional<SecurityLabel>& /*label*/, const String& /*avatarPath*/) {};
			virtual void addSystemMessage(const String& /*message*/) {};
			virtual void addErrorMessage(const String& /*message*/) {};

			virtual void setName(const String& name) {name_ = name;};
			virtual void show() {};
			virtual void activate() {};
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabel>& labels) {labels_ = labels;};
			virtual void setSecurityLabelsEnabled(bool enabled) {labelsEnabled_ = enabled;};
			virtual void setUnreadMessageCount(int /*count*/) {};
			virtual void convertToMUC() {};
			virtual TreeWidget *getTreeWidget() {return NULL;};
			virtual void setSecurityLabelsError() {};
			virtual SecurityLabel getSelectedSecurityLabel() {return SecurityLabel();};
			virtual void setInputEnabled(bool /*enabled*/) {};

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const String&)> onSendMessageRequest;

			String name_;
			std::vector<SecurityLabel> labels_;
			bool labelsEnabled_;
	};
}

