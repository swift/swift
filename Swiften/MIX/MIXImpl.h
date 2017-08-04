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
            virtual ~MIXImpl();

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

            virtual void updateSubscription(const std::unordered_set<std::string>& nodes) override;

            virtual void requestPreferencesForm() override;

            virtual void updatePreferences(Form::ref form) override;

        private:
            void handleUpdateSubscriptionResponse(MIXUpdateSubscription::ref, ErrorPayload::ref);
            void handlePreferencesFormReceived(MIXUserPreference::ref, ErrorPayload::ref);
            void handlePreferencesResultReceived(MIXUserPreference::ref /*payload*/, ErrorPayload::ref error);

        private:
            JID ownJID_;
            JID channelJID_;
            IQRouter* iqRouter_;
    };
}
