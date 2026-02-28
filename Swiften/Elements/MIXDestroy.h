/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API MIXDestroy : public Payload {
        public:
            using ref = std::shared_ptr<MIXDestroy>;

        public:
            MIXDestroy() {}
            virtual ~MIXDestroy() {}

            const std::string& getChannel() const {
                return channel_;
            }

            void setChannel(const std::string& channel) {
                this->channel_ = channel;
            }

        private:
            std::string channel_;
    };
}
