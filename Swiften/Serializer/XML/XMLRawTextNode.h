/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/XML/XMLNode.h>

namespace Swift {
    class SWIFTEN_API XMLRawTextNode : public XMLNode {
        public:
            XMLRawTextNode(const std::string& text) : text_(text) {
            }

            std::string serialize() {
                return text_;
            }

        private:
            std::string text_;
    };
}
