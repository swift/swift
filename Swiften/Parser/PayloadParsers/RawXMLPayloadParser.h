/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/SerializingParser.h>

namespace Swift {
    class SerializingParser;

    class SWIFTEN_API RawXMLPayloadParser : public GenericPayloadParser<RawXMLPayload> {
        public:
            RawXMLPayloadParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            int level_;
            SerializingParser serializingParser_;
    };
}
