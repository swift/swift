/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MIXInvitation.h>

namespace Swift {
    class SWIFTEN_API MIXInvitationAck : public Payload {

        public:
            using ref = std::shared_ptr<MIXInvitationAck>;

            enum class InvitationStatus {Joined, Declined, Acknowledged};

        public:

            MIXInvitationAck() {}

            const MIXInvitation::ref getInvitation() const {
                return invitation_;
            }

            void setInvitation(MIXInvitation::ref invitation) {
                invitation_ = invitation;
            }

            InvitationStatus getInvitationStatus() const {
                return status_;
            }

            void setInvitationStatus(InvitationStatus status) {
                status_ = status;
            }

        private:
            InvitationStatus status_;
            MIXInvitation::ref invitation_;
    };
}
