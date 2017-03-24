/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class StanzaChannel;
    class EntityCapsProvider;
    class TimerFactory;
    class Timer;


    class SWIFTEN_API ChatStateNotifier {
        public:
            ChatStateNotifier(StanzaChannel* stanzaChannel, const JID& contact, EntityCapsProvider* entityCapsManager, TimerFactory* timerFactory, int idleTimeInMilliSecs);
            ~ChatStateNotifier();

            void setContact(const JID& contact);

            void addChatStateRequest(Message::ref message);

            void setUserIsTyping();
            void userSentMessage();
            void userCancelledNewMessage();

            void receivedMessageFromContact(bool hasActiveElement);
            void setContactIsOnline(bool online);

        private:
            void userBecameIdleWhileTyping();
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
            std::shared_ptr<Timer> idleTimer_;

    };
}
