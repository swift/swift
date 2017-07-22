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
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Elements/MIXUserPreference.h>
#include <Swiften/Elements/MIXInvite.h>
#include <Swiften/Elements/MIXInvitation.h>
#include <Swiften/Elements/MIXInvitationAck.h>
#include <Swiften/Elements/ErrorPayload.h>

namespace Swift {
    class SWIFTEN_API MIX {
        public:
            using ref = std::shared_ptr<MIX>;

        public:
            virtual ~MIX();

            /**
            * Join Channel with preferences and subscriptions to node with invitation.
            */
            virtual void joinChannel(const std::unordered_set<std::string>& nodes, Form::ref form, MIXInvitation::ref invitation) = 0;

            /**
            * Join a MIX channel and subscribe to nodes.
            */
            virtual void joinChannelWithSubscriptions(const std::unordered_set<std::string>& nodes) = 0;

            /**
            * Join Channel with preferences and subscriptions to node with invitation.
            */
            virtual void joinChannelWithPreferences(const std::unordered_set<std::string>& nodes, Form::ref form) = 0;

            /**
            * Join channel with invite and subscribe to nodes.
            */
            virtual void joinChannelWithInvite(const std::unordered_set<std::string>& nodes, MIXInvitation::ref invitation) = 0;

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
            * Request invitation payload along with token for invitee.
            */
            virtual void requestInvitation(const JID& invitee) = 0;

            /**
            * Invite invitee based on received token from requestInvitation.
            */
            virtual void sendInvitation(MIXInvitation::ref invitation, std::string invitationMessage) = 0;

        public:
            boost::signals2::signal<void (MIXJoin::ref /* joinResponse */)> onJoinComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* joinError */)> onJoinFailed;
            boost::signals2::signal<void (MIXLeave::ref /* leaveResponse */)> onLeaveComplete;
            boost::signals2::signal<void (ErrorPayload::ref /* leaveError */)> onLeaveFailed;
            boost::signals2::signal<void (MIXUpdateSubscription::ref /* updateResponse */)> onSubscriptionUpdated;
            boost::signals2::signal<void (ErrorPayload::ref /* updateError */)> onSubscriptionUpdateFailed;
            boost::signals2::signal<void (Form::ref /* preferencesForm */)> onPreferencesFormReceived;
            boost::signals2::signal<void (ErrorPayload::ref /* failedConfiguration */)> onPreferencesUpdateFailed;
            boost::signals2::signal<void (MIXInvitation::ref /* inviteResponse */)> onInvitationReceived;
            boost::signals2::signal<void (ErrorPayload::ref /* failedConfiguration */)> onInvitationRequestFailed;
            boost::signals2::signal<void (Message::ref /* message */)> onMessageReceived;
    };
}
