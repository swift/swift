/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/MIXInvitation.h>

namespace Swift {
    class SWIFTEN_API MIXInvite : public Payload {

        public:
            using ref = std::shared_ptr<MIXInvite>;

        public:

            MIXInvite() {}

            const boost::optional<MIXInvitation::ref>& getInvitation() const {
                return invitation_;
            }

            void setInvitation(MIXInvitation::ref invitation) {
                invitation_ = invitation;
            }

            const boost::optional<JID>& getInvitee() const {
                return invitee_;
            }

            void setInvitee(JID invitee) {
                invitee_ = invitee;
            }

        private:
            boost::optional<JID> invitee_;
            boost::optional<MIXInvitation::ref> invitation_;
    };
}
