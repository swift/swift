/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_CHATWINDOW_H
#define SWIFTEN_CHATWINDOW_H

#include <boost/optional.hpp>
#include "Swiften/Base/boost_bsignals.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>

#include <string>
#include "Swiften/Elements/SecurityLabelsCatalog.h"
#include "Swiften/Elements/ChatState.h"

namespace Swift {
	class AvatarManager;
	class TreeWidget;
	class Roster;
	class TabComplete;

	class ChatWindow {
		public:
			enum AckState {Pending, Received, Failed};
			ChatWindow() {}
			virtual ~ChatWindow() {};

			/** Add message to window.
			 * @return id of added message (for acks).
			 */
			virtual std::string addMessage(const std::string& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;
			/** Adds action to window.
			 * @return id of added message (for acks);
			 */
			virtual std::string addAction(const std::string& message, const std::string& senderName, bool senderIsSelf, boost::shared_ptr<SecurityLabel> label, const std::string& avatarPath, const boost::posix_time::ptime& time) = 0;
			virtual void addSystemMessage(const std::string& message) = 0;
			virtual void addPresenceMessage(const std::string& message) = 0;
			virtual void addErrorMessage(const std::string& message) = 0;

			virtual void setContactChatState(ChatState::ChatStateType state) = 0;
			virtual void setName(const std::string& name) = 0;
			virtual void show() = 0;
			virtual void activate() = 0;
			virtual void setAvailableSecurityLabels(const std::vector<SecurityLabelsCatalog::Item>& labels) = 0;
			virtual void setSecurityLabelsEnabled(bool enabled) = 0;
			virtual void setUnreadMessageCount(int count) = 0;
			virtual void convertToMUC() = 0;
//			virtual TreeWidget *getTreeWidget() = 0;
			virtual void setSecurityLabelsError() = 0;
			virtual SecurityLabelsCatalog::Item getSelectedSecurityLabel() = 0;
			virtual void setInputEnabled(bool enabled) = 0;
			virtual void setRosterModel(Roster* model) = 0;
			virtual void setTabComplete(TabComplete* completer) = 0;
			virtual void replaceLastMessage(const std::string& message) = 0;
			virtual void setAckState(const std::string& id, AckState state) = 0;
			virtual void flash() = 0;

			boost::signal<void ()> onClosed;
			boost::signal<void ()> onAllMessagesRead;
			boost::signal<void (const std::string&)> onSendMessageRequest;
			boost::signal<void ()> onUserTyping;
			boost::signal<void ()> onUserCancelsTyping;
	};
}
#endif

