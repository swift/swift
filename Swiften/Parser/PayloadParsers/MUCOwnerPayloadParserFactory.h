/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;

    class SWIFTEN_API MUCOwnerPayloadParserFactory : public PayloadParserFactory {
        public:
            MUCOwnerPayloadParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return element == "query" && ns == "http://jabber.org/protocol/muc#owner";
            }

            virtual PayloadParser* createPayloadParser() {
                return new MUCOwnerPayloadParser(factories);
            }

        private:
            PayloadParserFactoryCollection* factories;

    };
}
