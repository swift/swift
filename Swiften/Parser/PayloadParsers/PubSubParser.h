/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSub.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class PayloadParser;
    class PubSubOptions;
    class PubSubConfigure;

    class SWIFTEN_API PubSubParser : public GenericPayloadParser<PubSub> {
        public:
            PubSubParser(PayloadParserFactoryCollection* parsers);
            virtual ~PubSubParser() SWIFTEN_OVERRIDE;

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            PayloadParserFactoryCollection* parsers;
            int level;
            std::shared_ptr<PayloadParser> currentPayloadParser;
            std::shared_ptr<PubSubConfigure> configurePayload;
            std::shared_ptr<PubSubOptions> optionsPayload;
    };
}
