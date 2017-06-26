/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MIXSetNickParser.h>

namespace Swift {
    class SWIFTEN_API MIXSetNickParserFactory : public PayloadParserFactory {
        public:
            MIXSetNickParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const SWIFTEN_OVERRIDE {
                return element == "setnick" && ns == "urn:xmpp:mix:0";
            }

            virtual PayloadParser* createPayloadParser() SWIFTEN_OVERRIDE {
                return new MIXSetNickParser();
            }
    };
}
