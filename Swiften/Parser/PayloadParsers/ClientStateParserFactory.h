/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ClientStateParser.h>

namespace Swift {
    class PayloadParserFactoryCollection;

    class SWIFTEN_API ClientStateParserFactory : public PayloadParserFactory {
        public:
            ClientStateParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return ns == "urn:xmpp:csi:0" &&
                    (element == "active" || element == "inactive");
            }

            virtual PayloadParser* createPayloadParser() {
                return new ClientStateParser();
            }

    };
}
