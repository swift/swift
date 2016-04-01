/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
    class SWIFTEN_API UnknownPayloadParser : public PayloadParser {
        public:
            UnknownPayloadParser() {}

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {}
            virtual void handleEndElement(const std::string&, const std::string&) {}
            virtual void handleCharacterData(const std::string&) {}

            virtual std::shared_ptr<Payload> getPayload() const {
                return std::shared_ptr<Payload>();
            }
    };
}
