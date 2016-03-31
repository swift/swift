/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Body : public Payload {
        public:
            Body(const std::string& text = "") : text_(text) {
            }

            void setText(const std::string& text) {
                text_ = text;
            }

            const std::string& getText() const {
                return text_;
            }

        private:
            std::string text_;
    };
}
