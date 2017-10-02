/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/String.h>
#include <Swiften/Serializer/XML/XMLNode.h>

namespace Swift {
    class SWIFTEN_API XMLTextNode : public XMLNode {
        public:
            typedef std::shared_ptr<XMLTextNode> ref;

            XMLTextNode(const std::string& text) : text_(text) {
                String::replaceAll(text_, '&', "&amp;"); // Should come first
                String::replaceAll(text_, '<', "&lt;");
                String::replaceAll(text_, '>', "&gt;");
            }

            std::string serialize() {
                return text_;
            }

            static ref create(const std::string& text) {
                return ref(new XMLTextNode(text));
            }

        private:
            std::string text_;
    };
}
