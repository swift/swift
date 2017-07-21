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
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Elements/MIXUserPreference.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
    class SWIFTEN_API MIX {
        public:
            using ref = std::shared_ptr<MIX>;

            static const std::string JIDMapNode;

        public:
            virtual ~MIX();

            /**
            * Join a MIX channel and subscribe to nodes.
            */
            virtual void joinChannel(const std::unordered_set<std::string>& nodes) = 0;

            /**
            * Join Channel with a set of preferences.
            */
            virtual void joinChannelWithPreferences(const std::unordered_set<std::string>& nodes, Form::ref form) = 0;

            /**
            * Update subscription of nodes.
            */
            virtual void updateSubscription(const std::unordered_set<std::string>& nodes) = 0;

            /**
            * Leave a MIX channel and unsubcribe nodes.
            */
            virtual void leaveChannel() = 0;

            /**
            * Request a configuration form for updating preferences.
            */
            virtual void requestPreferencesForm() = 0;

            /**
            * Update preferences after requesting preference form.
            */
            virtual void updatePreferences(Form::ref form) = 0;

            /**
            * Look up real JID from proxy JID in JIDVisible or JIDMaybeVisible Rooms.
            */
            virtual void lookupJID(const JID& proxyJID) = 0;

        public:
            boost::signals2::signal<void (MIXJoin::ref /* joinResponse */)> onJoinComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* joinError */)> onJoinFailed;
            boost::signals2::signal<void (MIXLeave::ref /* leaveResponse */)> onLeaveComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* leaveError */)> onLeaveFailed;
            boost::signals2::signal<void (MIXUpdateSubscription::ref /* updateResponse */)> onSubscriptionUpdated;
            boost::signals2::signal<void (ErrorPayload::ref /* updateError */)> onSubscriptionUpdateFailed;
            boost::signals2::signal<void (Form::ref /* preferencesForm */)> onPreferencesFormReceived;
            boost::signals2::signal<void (ErrorPayload::ref /* failedConfiguration */)> onPreferencesUpdateFailed;
            boost::signals2::signal<void (const JID& /* realJID */)> onLookupSuccess;
            boost::signals2::signal<void (ErrorPayload::ref /* failedConfiguration */)> onLookupFailed;

    };
}
