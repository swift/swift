/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
    class SWIFTEN_API UnknownPayloadParser : public PayloadParser {
        public:
            UnknownPayloadParser() {}

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&) {}
            virtual void handleEndElement(const std::string&, const std::string&) {}
            virtual void handleCharacterData(const std::string&) {}

            virtual boost::shared_ptr<Payload> getPayload() const {
                return boost::shared_ptr<Payload>();
            }
    };
}
