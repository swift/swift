/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API RawXMLPayload : public Payload {
        public:
            RawXMLPayload(const std::string& data = "") : rawXML_(data) {}

            void setRawXML(const std::string& data) {
                rawXML_ = data;
            }

            const std::string& getRawXML() const {
                return rawXML_;
            }

        private:
            std::string rawXML_;
    };
}
