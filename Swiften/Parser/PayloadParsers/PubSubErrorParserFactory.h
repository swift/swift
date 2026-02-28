/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PubSubErrorParser.h>

namespace Swift {
    class SWIFTEN_API PubSubErrorParserFactory : public PayloadParserFactory {
        public:
            PubSubErrorParserFactory() {
            }
            ~PubSubErrorParserFactory();

            virtual bool canParse(const std::string&, const std::string& ns, const AttributeMap&) const {
                return ns == "http://jabber.org/protocol/pubsub#errors";
            }

            virtual PayloadParser* createPayloadParser() {
                return new PubSubErrorParser();
            }
    };
}


