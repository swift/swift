/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;

    class SWIFTEN_API PrivateStorageParser : public GenericPayloadParser<PrivateStorage> {
        public:
            PrivateStorageParser(PayloadParserFactoryCollection* factories);

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
