/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>

namespace Swift {

    class SWIFTEN_API FormText{

        public:
            typedef std::shared_ptr<FormText> text;
            FormText();
            virtual ~FormText();
            void setTextString(const std::string& text);
            const std::string& getTextString() const;

        private:
            std::string text_;
    };
}
