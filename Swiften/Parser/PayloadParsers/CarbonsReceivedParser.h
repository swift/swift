/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsReceived.h>
#include <Swiften/Parser/GenericPayloadParser.h>
#include <Swiften/Parser/PayloadParsers/ForwardedParser.h>

namespace Swift {
    class SWIFTEN_API CarbonsReceivedParser : public GenericPayloadParser<CarbonsReceived> {
        public:
            CarbonsReceivedParser(PayloadParserFactoryCollection* factories);
            virtual ~CarbonsReceivedParser();
            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string&);

        private:
            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };


        private:
            PayloadParserFactoryCollection* factories_;
            std::shared_ptr<ForwardedParser> forwardedParser_;
            int level_;
    };
}
