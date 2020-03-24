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
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/DiscoItems.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Elements/MIXUserPreference.h>
#include <Swiften/Elements/MIXRegisterNick.h>
#include <Swiften/Elements/MIXSetNick.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
    class SWIFTEN_API MIX {
        public:
            using ref = std::shared_ptr<MIX>;

            static const std::string AllowedNode;
            static const std::string BannedNode;
            static const std::string ConfigurationNode;
            static const std::string InformationNode;
            static const std::string JIDMapNode;
            static const std::string JIDMaybeVisibleMapNode;
            static const std::string MessagesNode;
            static const std::string ParticipantsNode;
            static const std::string PresenceNode;

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
            * Register nick with MIX Channel.
            */
            virtual void registerNick(const std::string& nick) = 0;

            /**
            * Set/Update nick on MIX channel.
            */
            virtual void setNick(const std::string& nick) = 0;

            /**
            * Get features supported by MIX Channel.
            */
            virtual void requestSupportedFeatures() = 0;

            /**
            * Get nodes supported by MIX Channel.
            */
            virtual void requestSupportedNodes() = 0;

            /**
            * Get information about MIX Channel.
            */
            virtual void requestChannelInformation() = 0;

        public:
            boost::signals2::signal<void (MIXJoin::ref /* joinResponse */, ErrorPayload::ref /* joinError */)> onJoinResponse;
            boost::signals2::signal<void (MIXLeave::ref /* leaveResponse */, ErrorPayload::ref /* leaveError */)> onLeaveResponse;
            boost::signals2::signal<void (MIXUpdateSubscription::ref /* updateResponse */, ErrorPayload::ref /* updateError */)> onSubscriptionUpdateResponse;
            boost::signals2::signal<void (Form::ref /* preferencesForm */, ErrorPayload::ref /* failedConfiguration */)> onPreferencesFormResponse;
            boost::signals2::signal<void (MIXUserPreference::ref /* userPreferenceResponse */, ErrorPayload::ref /* failedUpdate */)> onPreferencesUpdateResponse;
            boost::signals2::signal<void (const std::string& /*nick*/, ErrorPayload::ref /* nickError */)> onNickResponse;
    };
}
