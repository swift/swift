/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubError.h>
#include <Swiften/Parser/EnumParser.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API PubSubErrorParser : public GenericPayloadParser<PubSubError> {
        public:
            PubSubErrorParser();
            virtual ~PubSubErrorParser() SWIFTEN_OVERRIDE;

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            int level;
            EnumParser<PubSubError::Type> typeParser;
            EnumParser<PubSubError::UnsupportedFeatureType> unsupportedTypeParser;
    };
}
