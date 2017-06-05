/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXInformation.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class PayloadParser;

    class SWIFTEN_API MIXInformationParser : public GenericPayloadParser<MIXInformation> {
        public:
            MIXInformationParser(PayloadParserFactoryCollection* parsers);
            virtual ~MIXInformationParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            int level;
            std::shared_ptr<PayloadParser> currentPayloadParser;
    };
}
