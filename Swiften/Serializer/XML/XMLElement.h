/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/XML/XMLNode.h>

namespace Swift {
    class SWIFTEN_API XMLElement : public XMLNode {
        public:
            typedef std::shared_ptr<XMLElement> ref;

            XMLElement(const std::string& tag, const std::string& xmlns = "", const std::string& text = "");

            void setAttribute(const std::string& attribute, const std::string& value);
            void addNode(std::shared_ptr<XMLNode> node);

            virtual std::string serialize();

        private:
            std::string tag_;
            std::map<std::string, std::string> attributes_;
            std::vector< std::shared_ptr<XMLNode> > childNodes_;
    };
}
