/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MUCOwnerPayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;

    class SWIFTEN_API MUCOwnerPayloadParser : public GenericPayloadParser<MUCOwnerPayload> {
        public:
        MUCOwnerPayloadParser(PayloadParserFactoryCollection* factories);

        private:
            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            PayloadParserFactoryCollection* factories;
            int level;
            std::shared_ptr<PayloadParser> currentPayloadParser;
    };
}
