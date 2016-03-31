/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;
    class StanzaParser;
    class DelayParser;

    class SWIFTEN_API ForwardedParser : public GenericPayloadParser<Forwarded> {
        public:
            ForwardedParser(PayloadParserFactoryCollection* factories);

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };

        private:
            PayloadParserFactoryCollection* factories_;
            boost::shared_ptr<StanzaParser> childParser_;
            boost::shared_ptr<DelayParser> delayParser_;
            int level_;
    };
}
