/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class StanzaChannel;
	class EntityCapsProvider;

	class ChatStateNotifier {
		public:
			ChatStateNotifier(StanzaChannel* stanzaChannel, const JID& contact, EntityCapsProvider* entityCapsManager);
			~ChatStateNotifier();

			void setContact(const JID& contact);

			void addChatStateRequest(Message::ref message);

			void setUserIsTyping();
			void userSentMessage();
			void userCancelledNewMessage();

			void receivedMessageFromContact(bool hasActiveElement);
			void setContactIsOnline(bool online);

		private:
			bool contactShouldReceiveStates();
			void changeState(ChatState::ChatStateType type);
			void handleCapsChanged(const JID& contact);

		private:
			StanzaChannel* stanzaChannel_;
			EntityCapsProvider* entityCapsManager_;
			JID contact_;
			bool contactHas85Caps_;
			bool contactHasSentActive_;
			bool userIsTyping_;
			bool contactIsOnline_;
	};
}
