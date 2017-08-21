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
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Elements/PubSubItem.h>
#include <Swiften/Elements/PubSubItems.h>
#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Elements/MIXUserPreference.h>
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
            * Update subscription of nodes.
            */
            virtual void updateSubscription(const std::unordered_set<std::string>& nodes) = 0;

            /**
            * Request a configuration form for updating preferences.
            */
            virtual void requestPreferencesForm() = 0;

            /**
            * Update preferences after requesting preference form.
            */
            virtual void updatePreferences(Form::ref form) = 0;

            /**
            * Sends a message to channel with given body.
            */
            virtual void sendMessage(const std::string &body) = 0;

            /**
            * Look up real JIDs from proxy JIDs in JIDVisible or JIDMaybeVisible Rooms.
            */
            virtual void lookupJID(const std::unordered_set<std::string>& proxyJIDs) = 0;

            /**
            * Retrieves the list of participants of the channel.
            */
            virtual void requestParticipantList() = 0;

        public:
            boost::signals2::signal<void (MIXUpdateSubscription::ref /* updateResponse */, ErrorPayload::ref /* updateError */)> onSubscriptionUpdateResponse;
            boost::signals2::signal<void (Form::ref /* preferencesForm */, ErrorPayload::ref /* failedConfiguration */)> onPreferencesFormResponse;
            boost::signals2::signal<void (MIXUserPreference::ref /* userPreferenceResponse */, ErrorPayload::ref /* failedUpdate */)> onPreferencesUpdateResponse;
            boost::signals2::signal<void (std::shared_ptr<PubSub> /* responsePubSub */, ErrorPayload::ref /* lookupError */)> onLookupResponse;
            boost::signals2::signal<void (std::shared_ptr<PubSub> /* responsePubSub */, ErrorPayload::ref /* lookupError */)> onParticipantResponse;
    };
}
