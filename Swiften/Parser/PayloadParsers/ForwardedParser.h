/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class StanzaParser;
    class DelayParser;

    class SWIFTEN_API ForwardedParser : public GenericPayloadParser<Forwarded> {
        public:
            ForwardedParser(PayloadParserFactoryCollection* factories);

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) override;
            virtual void handleEndElement(const std::string& element, const std::string&) override;
            virtual void handleCharacterData(const std::string& data) override;

            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };

        private:
            PayloadParserFactoryCollection* factories_;
            std::shared_ptr<StanzaParser> childParser_;
            std::shared_ptr<DelayParser> delayParser_;
            int level_;
    };
}
