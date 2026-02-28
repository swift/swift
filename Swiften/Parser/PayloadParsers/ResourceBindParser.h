/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ResourceBind.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API ResourceBindParser : public GenericPayloadParser<ResourceBind> {
        public:
            ResourceBindParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            int level_;
            bool inJID_;
            bool inResource_;
            std::string text_;
    };
}
