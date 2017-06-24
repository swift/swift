/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ChatStateParser.h>

namespace Swift {
    class SWIFTEN_API ChatStateParserFactory : public PayloadParserFactory {
        public:
            ChatStateParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return ns == "http://jabber.org/protocol/chatstates" &&
                    (element == "active" || element == "composing"
                     || element == "paused" || element == "inactive" || element == "gone");
            }

            virtual PayloadParser* createPayloadParser() {
                return new ChatStateParser();
            }

    };
}
