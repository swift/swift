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
    class SWIFTEN_API MIXRetract : public Payload {
        public:
            using ref = std::shared_ptr<MIXRetract>;

        public:

            MIXRetract() {}

            const std::string& getMessageID() const {
                return messageID_;
            }

            void setMessageID(std::string messageID) {
                messageID_ = messageID;
            }

        private:
            std::string messageID_;
    };
}
