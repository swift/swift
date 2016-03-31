/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/ChatState.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
    class SWIFTEN_API ChatStateTracker {
        public:
            ChatStateTracker();
            void handleMessageReceived(boost::shared_ptr<Message> message);
            void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
            boost::signal<void (ChatState::ChatStateType)> onChatStateChange;
        private:
            void changeState(ChatState::ChatStateType state);
            ChatState::ChatStateType currentState_;
    };
}
