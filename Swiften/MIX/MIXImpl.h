/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

 /*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/MIX/MIX.h>

namespace Swift {
    class StanzaChannel;
    class IQRouter;

    class SWIFTEN_API MIXImpl : public MIX {
        public:
            using ref = std::shared_ptr<MIXImpl>;

        public:
            MIXImpl(const JID& ownJID, const JID& channelJID, IQRouter* iqRouter);
            virtual ~MIXImpl() override;

            /**
             * Returns the (bare) JID of the user.
             */
            virtual JID getJID() const {
                return ownJID_.toBare();
            }

            /**
             * Returns the JID of MIX channel.
             */
            virtual JID getChannelJID() const {
                return channelJID_;
            }

            virtual void joinChannel(const std::unordered_set<std::string>& nodes) override;

            virtual void joinChannelWithPreferences(const std::unordered_set<std::string>& nodes, Form::ref form) override;

            virtual void updateSubscription(const std::unordered_set<std::string>& nodes) override;

            virtual void leaveChannel() override;

            virtual void requestPreferencesForm() override;

            virtual void updatePreferences(Form::ref form) override;

            virtual void registerNick(const std::string& nick) override;

            virtual void setNick(const std::string& nick) override;

            virtual void requestSupportedFeatures() override;

            virtual void requestSupportedNodes() override;

            virtual void requestChannelInformation() override;

        private:
            void handleJoinResponse(MIXJoin::ref, ErrorPayload::ref);
            void handleLeaveResponse(MIXLeave::ref, ErrorPayload::ref);
            void handleUpdateSubscriptionResponse(MIXUpdateSubscription::ref, ErrorPayload::ref);
            void handlePreferencesFormReceived(MIXUserPreference::ref, ErrorPayload::ref);
            void handlePreferencesResultReceived(MIXUserPreference::ref, ErrorPayload::ref);
            void handleNickAssigned(MIXRegisterNick::ref, ErrorPayload::ref);
            void handleNickUpdated(MIXSetNick::ref, ErrorPayload::ref);
            void handleFeaturesReceived(DiscoInfo::ref, ErrorPayload::ref);
            void handleSupportedNodesReceived(DiscoItems::ref, ErrorPayload::ref);
            void handleChannelInformation(std::shared_ptr<PubSub> payload, ErrorPayload::ref error);

        private:
            JID ownJID_;
            JID channelJID_;
            IQRouter* iqRouter_;
            DiscoInfo::ref supportedFeatures_;
            std::vector<std::string> supportedNodes_;
            Form::ref channelInformation_;
    };
}
