/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API Thread : public Payload {
        public:
            Thread(const std::string& text = "", const std::string& parent = "");
            virtual ~Thread();
            void setText(const std::string& text);
            const std::string& getText() const;
            void setParent(const std::string& parent);
            const std::string& getParent() const;

        private:
            std::string text_;
            std::string parent_;
    };
}
