#ifndef SWIFTEN_CHATWINDOW_H
#define SWIFTEN_CHATWINDOW_H

#include <boost/optional.hpp>
#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/SecurityLabel.h"

namespace Swift {
	class AvatarManager;
	class TreeWidget;

	class ChatWindow {
		public:
			ChatWindow() {}
			virtual ~ChatWindow() {};

			virtual void addMessage(const String& message, const String& senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) = 0;
			virtual void addAction(const String& message, const String& senderName, bool senderIsSelf, const boost::optional<SecurityLabel>& label, const String& avatarPath) = 0;
			virtual void addSystemMessage(const String& message) = 0;
			virtual void addErrorMessage(const String& message) = 0;

			virtual void setName(const String& name) = 0;
			virtual void show() = 0;
			virtual void activate() = 0;
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabel>& labels) = 0;
			virtual void setSecurityLabelsEnabled(bool enabled) = 0;
			virtual void setUnreadMessageCount(int count) = 0;
			virtual void convertToMUC() = 0;
			virtual TreeWidget *getTreeWidget() = 0;
			virtual void setSecurityLabelsError() = 0;
			virtual SecurityLabel getSelectedSecurityLabel() = 0;
			virtual void setInputEnabled(bool enabled) = 0;

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const String&)> onSendMessageRequest;
	};
}
#endif

