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
    class SWIFTEN_API MIXRegisterNick : public Payload {
        public:
            using ref = std::shared_ptr<MIXRegisterNick>;

        public:

            MIXRegisterNick() {}

            const std::string& getNick() const {
                return nick_;
            }

            void setNick(const std::string& nick) {
                nick_ = nick;
            }

        private:
            std::string nick_;
    };
}
