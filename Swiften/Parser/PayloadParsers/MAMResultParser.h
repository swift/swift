/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MAMResult.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class ForwardedParser;

    class SWIFTEN_API MAMResultParser : public GenericPayloadParser<MAMResult> {
        public:
            MAMResultParser(PayloadParserFactoryCollection* factories);

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) override;
            virtual void handleEndElement(const std::string& element, const std::string&) override;
            virtual void handleCharacterData(const std::string& data) override;

            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };

        private:
            std::shared_ptr<ForwardedParser> payloadParser_;
            PayloadParserFactoryCollection* factories_;
            int level_;
    };
}
