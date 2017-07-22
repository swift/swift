/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API MIXInvitation : public Payload {

        public:
            using ref = std::shared_ptr<MIXInvitation>;

        public:

            MIXInvitation() {}

            const JID& getInviter() const {
                return inviter_;
            }

            void setInviter(JID inviter) {
                inviter_ = inviter;
            }

            const JID& getInvitee() const {
                return invitee_;
            }

            void setInvitee(JID invitee) {
                invitee_ = invitee;
            }

            const JID& getChannel() const {
                return channel_;
            }

            void setChannel(JID channel) {
                channel_ = channel;
            }

            const std::string& getToken() const {
                return token_;
            }

            void setToken(std::string token) {
                token_ = token;
            }

        private:
            JID inviter_;
            JID invitee_;
            JID channel_;
            std::string token_;
    };
}
