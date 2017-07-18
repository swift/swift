/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXRetract.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParser;

    class SWIFTEN_API MIXRetractParser : public GenericPayloadParser<MIXRetract> {
        public:
            MIXRetractParser();
            virtual ~MIXRetractParser() override;

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) override;
            virtual void handleEndElement(const std::string& element, const std::string&) override;
            virtual void handleCharacterData(const std::string& data) override;

        private:
            int level_;
    };
}
