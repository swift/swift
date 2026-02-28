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
    class SWIFTEN_API Subject : public Payload {
        public:
            Subject(const std::string& text = "") : text_(text) {
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
