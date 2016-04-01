/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/AttributeMap.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    class SWIFTEN_API SerializingParser {
        public:
            SerializingParser();

            void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
            void handleEndElement(const std::string& element, const std::string& ns);
            void handleCharacterData(const std::string& data);

            std::string getResult() const;

        private:
            std::vector< std::shared_ptr<XMLElement> > elementStack_;
            std::shared_ptr<XMLElement> rootElement_;
    };
}
