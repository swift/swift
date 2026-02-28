/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Nickname : public Payload {
        public:
            Nickname(const std::string& nickname = "") : nickname(nickname) {
            }

            void setNickname(const std::string& nickname) {
                this->nickname = nickname;
            }

            const std::string& getNickname() const {
                return nickname;
            }

        private:
            std::string nickname;
    };
}
