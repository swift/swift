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
#include <Swiften/Elements/Presence.h>

namespace Swift {
    class SWIFTEN_API ChatStateTracker {
        public:
            ChatStateTracker();
            void handleMessageReceived(std::shared_ptr<Message> message);
            void handlePresenceChange(std::shared_ptr<Presence> newPresence);
            boost::signals2::signal<void (ChatState::ChatStateType)> onChatStateChange;
        private:
            void changeState(ChatState::ChatStateType state);
            ChatState::ChatStateType currentState_;
    };
}
