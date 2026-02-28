/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PrivateStorageParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;

    class SWIFTEN_API PrivateStorageParserFactory : public PayloadParserFactory {
        public:
            PrivateStorageParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return element == "query" && ns == "jabber:iq:private";
            }

            virtual PayloadParser* createPayloadParser() {
                return new PrivateStorageParser(factories);
            }

        private:
            PayloadParserFactoryCollection* factories;

    };
}
