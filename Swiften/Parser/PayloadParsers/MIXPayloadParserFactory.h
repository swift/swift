/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MIXPayloadParser.h>

namespace Swift {
    class SWIFTEN_API MIXPayloadParserFactory : public PayloadParserFactory {
        public:
            MIXPayloadParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const SWIFTEN_OVERRIDE {
                return element == "mix" && ns == "urn:xmpp:mix:0";
            }

            virtual PayloadParser* createPayloadParser() SWIFTEN_OVERRIDE {
                return new MIXPayloadParser();
            }
    };
}
