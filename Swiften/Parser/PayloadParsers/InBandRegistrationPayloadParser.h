/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/InBandRegistrationPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class FormParserFactory;
    class FormParser;

    class SWIFTEN_API InBandRegistrationPayloadParser : public GenericPayloadParser<InBandRegistrationPayload> {
        public:
            InBandRegistrationPayloadParser();
            ~InBandRegistrationPayloadParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };
            int level;
            FormParserFactory* formParserFactory;
            FormParser* formParser;
            std::string currentText;
    };
}
