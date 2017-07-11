/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/UserLocation.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API UserLocationParser : public GenericPayloadParser<UserLocation> {
        public:
            UserLocationParser();
            virtual ~UserLocationParser() SWIFTEN_OVERRIDE;

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            int level;
            std::string currentText;
    };
}
