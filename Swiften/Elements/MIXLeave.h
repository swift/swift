/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXLeave : public Payload {
        public:
            using ref = std::shared_ptr<MIXLeave>;

        public:

            MIXLeave() {}

            const boost::optional<JID>& getChannel() const {
                return channel_;
            }

            void setChannel(const JID& channel) {
                channel_ = channel;
            }

        private:
            boost::optional<JID> channel_;
    };
}
