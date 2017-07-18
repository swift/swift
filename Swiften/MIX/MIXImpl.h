/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/MIX/MIX.h>

#include <Swiften/Base/IDGenerator.h>

namespace Swift {
    class StanzaChannel;
    class IQRouter;

    class SWIFTEN_API MIXImpl : public MIX {
        public:
            using ref = std::shared_ptr<MIXImpl>;

        public:
            MIXImpl(const JID &ownJID, IQRouter* iqRouter, StanzaChannel* stanzaChannel);
            virtual ~MIXImpl();

            /**
             * Returns the (bare) JID of the user.
             */
            virtual JID getJID() const {
                return ownJID_.toBare();
            }

            virtual void joinChannelAndSubscribe(const JID &channel, const std::unordered_set<std::string>& nodes) override;
            virtual void joinChannel(const JID &channel) override;

            virtual void leaveChannel(const JID &channel) override;

            virtual void sendMessage(const JID &channel, const std::string &body) override;
            virtual void retractMessage(const JID &channel, const std::string &messageID) override;

        private:
            void handleJoinResponse(MIXJoin::ref, ErrorPayload::ref);
            void handleLeaveResponse(MIXLeave::ref, ErrorPayload::ref);
            void handleIncomingMessage(Message::ref);

        private:
            JID ownJID_;
            IQRouter* iqRouter_;
            StanzaChannel* stanzaChannel_;
            IDGenerator idGenerator;
    };
}
