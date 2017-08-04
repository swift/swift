/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <unordered_set>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/MIX/MIXImpl.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {

    class SWIFTEN_API MIXRegistry {
        public:
            using MIXInstanceMap = std::map<JID, MIXImpl::ref>;

        public:
            MIXRegistry(const JID& ownJID, IQRouter* iqRouter, XMPPRoster* xmppRoster);

            ~MIXRegistry();

            void joinChannel(const JID& channelJID, const std::unordered_set<std::string>& nodes);

            void leaveChannel(const JID& channelJID);

            std::unordered_set<MIXImpl::ref> getChannels();

            MIX::ref getMIXInstance(const JID& jid);

        private:
            void handleJIDAdded(const JID& jid);
            void handleJIDRemoved(const JID& jid);
            void handleJoinResponse(MIXJoin::ref, ErrorPayload::ref);
            void handleLeaveResponse(MIXLeave::ref, ErrorPayload::ref);

        public:
            boost::signals2::signal<void (const JID& /* joinedChannel */)> onChannelJoined;
            boost::signals2::signal<void (ErrorPayload::ref /* joinResponse */)> onChannelJoinFailed;
            boost::signals2::signal<void (const JID& /* leavedChannel */)> onChannelLeft;

        private:
            JID ownJID_;
            IQRouter* iqRouter_;
            XMPPRoster* xmppRoster_;
            MIXInstanceMap entries_;
    };
}
