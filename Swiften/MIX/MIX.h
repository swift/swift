/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXPayload.h>
#include <Swiften/Elements/MIXRetract.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
    class SWIFTEN_API MIX {
        public:
            using ref = std::shared_ptr<MIX>;

        public:
            virtual ~MIX();

            /**
            * Join a MIX channel and subscribe to nodes.
            */
            virtual void joinChannelAndSubscribe(const JID &channel, const std::unordered_set<std::string>& nodes) = 0;
            virtual void joinChannel(const JID &channel) = 0;

            virtual void leaveChannel(const JID &channel) = 0;

            virtual void sendMessage(const JID &channel, const std::string &body) = 0;
            virtual void retractMessage(const JID &channel, const std::string &messageID) = 0;

        public:
            boost::signals2::signal<void (MIXJoin::ref /* joinResponse */)> onJoinComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* joinError */)> onJoinFailed;
            boost::signals2::signal<void (MIXLeave::ref /* leaveResponse */)> onLeaveComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* leaveError */)> onLeaveFailed;
            boost::signals2::signal<void (Message::ref /*message*/)> onMessageReceived;
    };
}
