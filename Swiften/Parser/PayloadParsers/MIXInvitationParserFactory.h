/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MIXInvitationParser.h>

namespace Swift {
    class SWIFTEN_API MIXInvitationParserFactory : public PayloadParserFactory {
        public:
            MIXInvitationParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const override {
                return element == "invitation" && ns == "urn:xmpp:mix:0";
            }

            virtual PayloadParser* createPayloadParser() override {
                return new MIXInvitationParser();
            }
    };
}
