/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/Tree/ParserElement.h>

namespace Swift {
    class SWIFTEN_API NullParserElement : public ParserElement {
        public:
            NullParserElement() : ParserElement("", "", AttributeMap()) {}

            virtual operator bool() { return false; }

            static std::shared_ptr<NullParserElement> element;
    };
}
