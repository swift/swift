/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h>

namespace Swift {
    class SWIFTEN_API MUCUserPayloadParserFactory : public PayloadParserFactory {
        public:
            MUCUserPayloadParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return element == "x" && ns == "http://jabber.org/protocol/muc#user";
            }

            virtual PayloadParser* createPayloadParser() {
                return new MUCUserPayloadParser(factories);
            }

        private:
            PayloadParserFactoryCollection* factories;
    };
}
