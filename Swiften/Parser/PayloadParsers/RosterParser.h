/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SerializingParser;

    class SWIFTEN_API RosterParser : public GenericPayloadParser<RosterPayload> {
        public:
            RosterParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            enum Level {
                TopLevel = 0,
                PayloadLevel = 1,
                ItemLevel = 2
            };
            int level_;
            bool inItem_;
            RosterItemPayload currentItem_;
            std::string currentText_;
            SerializingParser* unknownContentParser_;
    };
}
